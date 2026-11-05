// Copyright (C) HCI-BCU 2025. All rights reserved.

#include "Viewport/ViewportCameraController.h"
#include "Viewport/AccessionViewportSettings.h"
#include "AccessionLogging.h"

#include "EditorViewportClient.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "EngineUtils.h"
#include "Components/MeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Volume.h"

// Framing Helpers

namespace ViewportFraming
{
	/** Corners must sit at least this far in front of the camera, so nothing is clipped by the near plane. */
	static constexpr double NearMargin = 20.0;

	/** Gap left between the camera and an occluder when the camera has to be pulled in front of it. */
	static constexpr double OccluderMargin = 25.0;

	/** Radius of the sphere swept along the camera path, so the camera never hugs a wall or squeezes through a gap. */
	static constexpr double CameraProbeRadius = 12.0;

	/** Mesh bounds used as occluders are shrunk by this, as meshes rarely fill their box (e.g. a tree's canopy). */
	static constexpr double MeshBoundsScale = 0.8;

	/** Tangents of the half field of view, shrunk by the framing padding so the item does not touch the edges. */
	struct FLens
	{
		double TanHalfH = 1.0;
		double TanHalfV = 1.0;
	};

	static FLens GetLens(const FEditorViewportClient* Client, double Padding)
	{
		const FIntPoint ViewSize = Client->Viewport ? Client->Viewport->GetSizeXY() : FIntPoint(16, 9);
		const double Aspect = ViewSize.Y > 0 ? double(ViewSize.X) / double(ViewSize.Y) : 16.0 / 9.0;

		// Editor perspective FOV is horizontal.
		const double HalfH = FMath::DegreesToRadians(FMath::Clamp<double>(Client->ViewFOV, 5.0, 170.0) * 0.5);

		FLens Lens;
		Lens.TanHalfH = FMath::Tan(HalfH) / FMath::Max(Padding, 1.0);
		Lens.TanHalfV = Lens.TanHalfH / Aspect;
		return Lens;
	}

	/** Calls Func(Forward, Right, Up) offsets of each of the 8 box corners, in the camera basis of Rotation. */
	template <typename FuncType>
	static void ForEachCornerInView(const FBox& Box, const FRotator& Rotation, FuncType&& Func)
	{
		const FRotationMatrix Basis(Rotation);
		const FVector Forward = Basis.GetUnitAxis(EAxis::X);
		const FVector Right = Basis.GetUnitAxis(EAxis::Y);
		const FVector Up = Basis.GetUnitAxis(EAxis::Z);

		const FVector Extent = Box.GetExtent();
		for (int32 Corner = 0; Corner < 8; ++Corner)
		{
			const FVector Offset = Extent * FVector(
				(Corner & 1) ? 1.0 : -1.0,
				(Corner & 2) ? 1.0 : -1.0,
				(Corner & 4) ? 1.0 : -1.0);

			Func(FVector::DotProduct(Offset, Forward), FVector::DotProduct(Offset, Right), FVector::DotProduct(Offset, Up));
		}
	}

	/** Smallest distance back from the box centre (along Rotation) that keeps every corner in front of the camera. */
	static double GetNearDistance(const FBox& Box, const FRotator& Rotation)
	{
		double Distance = NearMargin;
		ForEachCornerInView(Box, Rotation, [&Distance](double X, double, double)
			{ Distance = FMath::Max(Distance, NearMargin - X); });
		return Distance;
	}

	/**
	 * Distance back from the box centre (along Rotation) at which all 8 corners of the box fit inside the view.
	 * Exact for the box, so long thin items are framed tightly instead of by their bounding sphere.
	 * Camera at Centre - Forward * D sees a corner at depth (D + X); it is inside the view when |Y| <= TanH * (D + X) and |Z| <= TanV * (D + X).
	 */
	static double GetFitDistance(const FBox& Box, const FRotator& Rotation, const FLens& Lens)
	{
		double Distance = GetNearDistance(Box, Rotation);
		ForEachCornerInView(Box, Rotation, [&Distance, &Lens](double X, double Y, double Z)
			{
				Distance = FMath::Max(Distance, FMath::Abs(Y) / Lens.TanHalfH - X);
				Distance = FMath::Max(Distance, FMath::Abs(Z) / Lens.TanHalfV - X); });
		return Distance;
	}

	/** Points on the item tested for line of sight: centre, top, and the middle of each side. Bottom is skipped, as it usually rests on the floor. */
	static void GetSamplePoints(const FBox& Box, TArray<FVector>& OutSamples)
	{
		const FVector Centre = Box.GetCenter();
		const FVector Half = Box.GetExtent() * 0.5;

		OutSamples = {
			Centre,
			Centre + FVector(0.0, 0.0, Half.Z),
			Centre + FVector(Half.X, 0.0, 0.0),
			Centre - FVector(Half.X, 0.0, 0.0),
			Centre + FVector(0.0, Half.Y, 0.0),
			Centre - FVector(0.0, Half.Y, 0.0),
		};
	}

	/**
 * Segment vs axis aligned box (slab test).
 * @param OutTime Fraction 0..1 along A->B where the segment enters the box (0 if A is inside).
 */
	static bool SegmentBoxHit(const FBox& Box, const FVector& A, const FVector& B, double& OutTime)
	{
		const FVector Delta = B - A;
		double TMin = 0.0, TMax = 1.0;

		for (int32 Axis = 0; Axis < 3; ++Axis)
		{
			if (FMath::Abs(Delta[Axis]) < UE_SMALL_NUMBER)
			{
				if (A[Axis] < Box.Min[Axis] || A[Axis] > Box.Max[Axis])
					return false;
				continue;
			}

			double T1 = (Box.Min[Axis] - A[Axis]) / Delta[Axis];
			double T2 = (Box.Max[Axis] - A[Axis]) / Delta[Axis];
			if (T1 > T2)
				Swap(T1, T2);

			TMin = FMath::Max(TMin, T1);
			TMax = FMath::Min(TMax, T2);
			if (TMin > TMax)
				return false;
		}

		OutTime = TMin;
		return true;
	}

	/**
	 * Line of sight tests between a focused item and candidate camera positions.
	 *
	 * Blockers are found two ways:
	 *  - Collision: traces on the Visibility channel, run in BOTH directions. A one-way trace misses single-sided
	 *    geometry seen from behind (planes, landscape from below) and anything the trace starts inside.
	 *  - Meshes without Visibility collision (foliage, props with collision off): tested against their oriented bounds.
	 *    Instanced meshes (foliage) are tested per instance.
	 */
	class FLineOfSight
	{
	public:
		FLineOfSight(UWorld* InWorld, const FBox& InTargetBox, const TArray<const AActor*>& TargetActors, double SearchRadius)
			: World(InWorld), TargetBox(InTargetBox), Params(SCENE_QUERY_STAT(AccessionFocusLineOfSight), /*bTraceComplex*/ true)
		{
			// The item itself (and anything framed with it) never counts as being in the way.
			for (const AActor* Actor : TargetActors)
			{
				if (Actor)
				{
					Params.AddIgnoredActor(Actor);
				}
			}

			Probe = FCollisionShape::MakeSphere(CameraProbeRadius);

			if (GetDefault<UAccessionViewportSettings>()->bMeshBoundsOcclusion)
			{
				GatherMeshOccluders(TargetActors, SearchRadius);
			}
		}

		/**
		 * @return Number of samples with a clear line to the camera.
		 *         0 if the camera path from the item's centre is blocked, or the camera would sit inside or against geometry.
		 */
		int32 CountVisibleSamples(const TArray<FVector>& Samples, const FVector& CameraLocation) const
		{
			// Samples[0] is the centre: the camera looks at it and the label sits on it.
			if (IsCameraPathBlocked(CameraLocation))
				return 0;

			int32 Visible = 1;
			for (int32 i = 1; i < Samples.Num(); ++i)
			{
				Visible += IsSegmentBlocked(Samples[i], CameraLocation) ? 0 : 1;
			}
			return Visible;
		}

		/** @return Distance from the item's centre to the nearest blocker on the way to the camera, or a negative value if clear. */
		double GetBlockingDistance(const FVector& CameraLocation) const
		{
			const FVector Centre = TargetBox.GetCenter();
			const FVector Start = GetExitPoint(CameraLocation);
			const double PathLength = FVector::Dist(Start, CameraLocation);

			double Nearest = -1.0;
			auto Consider = [&Nearest](double Distance)
				{
					if (Distance >= 0.0 && (Nearest < 0.0 || Distance < Nearest))
						Nearest = Distance;
				};

			FHitResult Hit;
			if (World->LineTraceSingleByChannel(Hit, Start, CameraLocation, ECC_Visibility, Params))
				Consider(Hit.Distance);

			// Geometry only visible from the camera side (e.g. single-sided walls facing away from the item).
			if (World->LineTraceSingleByChannel(Hit, CameraLocation, Start, ECC_Visibility, Params))
				Consider(PathLength - Hit.Distance);

			const double MeshTime = FindMeshOccluder(Start, CameraLocation);
			if (MeshTime >= 0.0)
				Consider(MeshTime * PathLength);

			return Nearest < 0.0 ? -1.0 : FVector::Dist(Centre, Start) + Nearest;
		}

	private:
		struct FMeshOccluder
		{
			FTransform Transform;
			FBox LocalBox;
		};

		bool IsSegmentBlocked(const FVector& A, const FVector& B) const
		{
			if (World->LineTraceTestByChannel(A, B, ECC_Visibility, Params) ||
				World->LineTraceTestByChannel(B, A, ECC_Visibility, Params))
				return true;

			return FindMeshOccluder(A, B) >= 0.0;
		}

		bool IsCameraPathBlocked(const FVector& CameraLocation) const
		{
			const FVector Start = GetExitPoint(CameraLocation);

			// Sweeps start penetrating when the camera is inside geometry, so this also rejects embedded cameras.
			if (World->SweepTestByChannel(Start, CameraLocation, FQuat::Identity, ECC_Visibility, Probe, Params) ||
				World->SweepTestByChannel(CameraLocation, Start, FQuat::Identity, ECC_Visibility, Probe, Params))
				return true;

			return FindMeshOccluder(Start, CameraLocation) >= 0.0;
		}

		/** @return Fraction 0..1 along A->B of the first non-colliding mesh hit, or a negative value if none. */
		double FindMeshOccluder(const FVector& A, const FVector& B) const
		{
			double Nearest = -1.0;
			for (const FMeshOccluder& Occluder : MeshOccluders)
			{
				// Fractions along the segment are unchanged by the (affine) transform into the mesh's local space.
				double Time;
				if (SegmentBoxHit(Occluder.LocalBox,
					Occluder.Transform.InverseTransformPosition(A),
					Occluder.Transform.InverseTransformPosition(B),
					Time) &&
					(Nearest < 0.0 || Time < Nearest))
				{
					Nearest = Time;
				}
			}
			return Nearest;
		}

		/** Point just outside the item's box, on the line from its centre to the camera. Path checks start here, not inside the item. */
		FVector GetExitPoint(const FVector& CameraLocation) const
		{
			const FVector Centre = TargetBox.GetCenter();
			const FVector Direction = (CameraLocation - Centre).GetSafeNormal();
			const FVector Extent = TargetBox.GetExtent();

			double ToSurface = TNumericLimits<double>::Max();
			for (int32 Axis = 0; Axis < 3; ++Axis)
			{
				if (FMath::Abs(Direction[Axis]) > UE_SMALL_NUMBER)
				{
					ToSurface = FMath::Min(ToSurface, Extent[Axis] / FMath::Abs(Direction[Axis]));
				}
			}

			if (ToSurface == TNumericLimits<double>::Max())
				ToSurface = 0.0;

			return Centre + Direction * (ToSurface + CameraProbeRadius + 1.0);
		}

		static FBox ShrinkBox(const FBox& Box)
		{
			return FBox::BuildAABB(Box.GetCenter(), Box.GetExtent() * MeshBoundsScale);
		}

		void GatherMeshOccluders(const TArray<const AActor*>& TargetActors, double SearchRadius)
		{
			const FSphere SearchSphere(TargetBox.GetCenter(), SearchRadius);
			const double MaxExtent = GetDefault<UAccessionViewportSettings>()->MaxTargetExtent;

			for (TActorIterator<AActor> It(World); It; ++It)
			{
				AActor* Actor = *It;
				if (!::IsValid(Actor) || Actor->IsHiddenEd() || Actor->IsA<AVolume>() || TargetActors.Contains(Actor))
					continue;

				Actor->ForEachComponent<UMeshComponent>(false, [&](UMeshComponent* Mesh)
					{
						if (!Mesh->IsRegistered() || !Mesh->IsVisible() || Mesh->IsEditorOnly() || Mesh->IsVisualizationComponent())
							return;

						// Meshes that block Visibility are already found by the traces.
						if (Mesh->IsQueryCollisionEnabled() && Mesh->GetCollisionResponseToChannel(ECC_Visibility) == ECR_Block)
							return;

						if (!Mesh->Bounds.GetSphere().Intersects(SearchSphere))
							return;

						if (const UInstancedStaticMeshComponent* Instanced = Cast<UInstancedStaticMeshComponent>(Mesh))
						{
							const UStaticMesh* StaticMesh = Instanced->GetStaticMesh();
							if (!StaticMesh)
								return;

							const FBoxSphereBounds MeshBounds = StaticMesh->GetBounds();
							const FBox InstanceBox = ShrinkBox(MeshBounds.GetBox());

							for (int32 Instance = 0; Instance < Instanced->GetInstanceCount(); ++Instance)
							{
								FTransform InstanceTransform;
								if (!Instanced->GetInstanceTransform(Instance, InstanceTransform, /*bWorldSpace*/ true))
									continue;

								const double Reach = SearchRadius + MeshBounds.SphereRadius * InstanceTransform.GetMaximumAxisScale();
								if (FVector::DistSquared(InstanceTransform.GetLocation(), SearchSphere.Center) <= FMath::Square(Reach))
								{
									MeshOccluders.Add({ InstanceTransform, InstanceBox });
								}
							}
							return;
						}

						// Very large meshes (sky spheres, backdrops) would block every view.
						if (Mesh->Bounds.SphereRadius > MaxExtent)
							return;

						MeshOccluders.Add({ Mesh->GetComponentTransform(), ShrinkBox(Mesh->CalcBounds(FTransform::Identity).GetBox()) }); });
			}
		}

		UWorld* World;
		FBox TargetBox;
		FCollisionQueryParams Params;
		FCollisionShape Probe;
		TArray<FMeshOccluder> MeshOccluders;
	};

	/**
	 * Orientations to try, closest to the direct approach first, to keep the user oriented.
	 * Alternative pitches are fixed angles around FocusPitch, never offsets from the current pitch,
	 * so repeated focuses cannot drift towards a top-down or upward view.
	 */
	static void GetCandidateRotations(const FRotator& Preferred, float FocusPitch, float MaxPitch, TArray<FRotator>& OutRotations)
	{
		struct FOffset
		{
			double Yaw, Pitch, Cost;
		};

		static const double YawOffsets[] = { 0.0, 30.0, -30.0, 60.0, -60.0, 90.0, -90.0, 135.0, -135.0, 180.0 };

		// Negative pitch looks down, i.e. raises the camera, which clears most low occluders (furniture, walls, foliage).
		const double Pitches[] = { Preferred.Pitch, FocusPitch - 20.0, FocusPitch - 40.0, FocusPitch + 15.0 };

		TArray<FOffset> Offsets;
		for (const double Yaw : YawOffsets)
		{
			for (const double Pitch : Pitches)
			{
				// Looking further down is preferred over swinging round, looking up from below is the last resort.
				const double Delta = Pitch - Preferred.Pitch;
				const double PitchCost = Delta <= 0.0 ? FMath::Abs(Delta) * 0.75 : Delta * 2.0;
				Offsets.Add({ Yaw, Pitch, FMath::Abs(Yaw) + PitchCost });
			}
		}

		Offsets.StableSort([](const FOffset& A, const FOffset& B)
			{ return A.Cost < B.Cost; });

		for (const FOffset& Offset : Offsets)
		{
			FRotator Rotation = Preferred;
			Rotation.Yaw = FRotator::NormalizeAxis(Rotation.Yaw + Offset.Yaw);
			Rotation.Pitch = FMath::Clamp<double>(Offset.Pitch, -MaxPitch, MaxPitch);
			Rotation.Roll = 0.0;

			if (!OutRotations.ContainsByPredicate([&Rotation](const FRotator& Existing)
				{ return Existing.Equals(Rotation, 1.0); }))
			{
				OutRotations.Add(Rotation);
			}
		}
	}

	static FViewportCameraPose MakePose(const FViewportCameraPose& Base, const FVector& Centre, const FRotator& Rotation, double Distance)
	{
		FViewportCameraPose Pose = Base;
		Pose.Rotation = Rotation;
		Pose.Location = Centre - Rotation.Vector() * Distance;
		Pose.LookAt = Centre;
		return Pose;
	}
}

// FViewportCameraPose

FViewportCameraPose FViewportCameraPose::Capture(const FEditorViewportClient* Client)
{
	FViewportCameraPose Pose;
	Pose.Location = Client->GetViewLocation();
	Pose.Rotation = Client->GetViewRotation();
	Pose.LookAt = Client->GetLookAtLocation();
	Pose.OrthoZoom = Client->GetOrthoZoom();
	return Pose;
}

void FViewportCameraPose::Apply(FEditorViewportClient* Client) const
{
	Client->SetViewLocation(Location);
	Client->SetViewRotation(Rotation);
	Client->SetLookAtLocation(LookAt, /*bRecalculateView*/ false);

	if (Client->IsOrtho())
	{
		Client->SetOrthoZoom(OrthoZoom);
	}

	Client->Invalidate();
}

bool FViewportCameraPose::NearlyEquals(const FViewportCameraPose& Other, double LocationTolerance, double RotationTolerance) const
{
	return Location.Equals(Other.Location, LocationTolerance) &&
		Rotation.Equals(Other.Rotation, RotationTolerance) &&
		FMath::IsNearlyEqual(OrthoZoom, Other.OrthoZoom, 1.f);
}

FViewportCameraPose FViewportCameraPose::Blend(const FViewportCameraPose& From, const FViewportCameraPose& To, float Alpha)
{
	FViewportCameraPose Out;
	Out.Location = FMath::Lerp(From.Location, To.Location, Alpha);
	Out.LookAt = FMath::Lerp(From.LookAt, To.LookAt, Alpha);
	Out.Rotation = FQuat::Slerp(From.Rotation.Quaternion(), To.Rotation.Quaternion(), Alpha).Rotator();
	Out.OrthoZoom = FMath::Lerp(From.OrthoZoom, To.OrthoZoom, Alpha);
	return Out;
}

// FViewportCameraController

void FViewportCameraController::FocusOnBox(FEditorViewportClient* Client, const FBox& Box, const TArray<const AActor*>& TargetActors)
{
	if (!Client || !Box.IsValid)
		return;

	const UAccessionViewportSettings* Settings = GetDefault<UAccessionViewportSettings>();

	PushHistory(GetIntendedPose(Client));
	bLastActionWasOrbit = false;

	if (Client->IsOrtho())
	{
		// The engine's own framing already handles orthographic zoom correctly, and ortho views see through walls anyway.
		bTransitioning = false;
		Client->FocusViewportOnBox(Box, /*bInstant*/ Settings->bReducedMotion);
		return;
	}

	const FViewportCameraPose Current = GetIntendedPose(Client);
	const FVector Centre = Box.GetCenter();
	const ViewportFraming::FLens Lens = ViewportFraming::GetLens(Client, Settings->FramingPadding);

	// Preferred view: approach from the current direction (yaw), so the user keeps their sense of direction.
	// If the camera is inside the item, keep facing the same way.
	const FVector ToTarget = Centre - Current.Location;
	FRotator Preferred = ToTarget.IsNearlyZero(1.0) ? Current.Rotation : ToTarget.Rotation();

	// Pitch stays near a steady eye level instead of following the camera, otherwise a focus that had to raise
	// the camera (to see over something) makes the next one start higher, drifting towards a top-down view.
	Preferred.Pitch = FMath::Clamp<double>(
		FMath::Clamp<double>(Preferred.Pitch,
			Settings->FocusPitch - Settings->FocusPitchTolerance,
			Settings->FocusPitch + Settings->FocusPitchTolerance),
		-Settings->MaxPitch, Settings->MaxPitch);
	Preferred.Roll = 0.0;

	FViewportCameraPose Target = ViewportFraming::MakePose(
		Current, Centre, Preferred, ViewportFraming::GetFitDistance(Box, Preferred, Lens));

	UWorld* World = Client->GetWorld();
	if (Settings->bEnsureLineOfSight && World)
	{
		Target = FindClearFraming(World, Box, Preferred, Lens, TargetActors, Target);
	}

	TransitionTo(Client, Target);
}

FViewportCameraPose FViewportCameraController::FindClearFraming(
	UWorld* World,
	const FBox& Box,
	const FRotator& Preferred,
	const ViewportFraming::FLens& Lens,
	const TArray<const AActor*>& TargetActors,
	const FViewportCameraPose& DirectPose) const
{
	using namespace ViewportFraming;

	const UAccessionViewportSettings* Settings = GetDefault<UAccessionViewportSettings>();
	const FVector Centre = Box.GetCenter();

	TArray<FVector> Samples;
	GetSamplePoints(Box, Samples);

	TArray<FRotator> Rotations;
	GetCandidateRotations(Preferred, Settings->FocusPitch, Settings->MaxPitch, Rotations);

	// Every candidate framing, fitted to the box, in order of preference.
	TArray<FViewportCameraPose> Poses;
	double SearchRadius = 0.0;
	for (const FRotator& Rotation : Rotations)
	{
		const double Distance = GetFitDistance(Box, Rotation, Lens);
		Poses.Add(MakePose(DirectPose, Centre, Rotation, Distance));
		SearchRadius = FMath::Max(SearchRadius, Distance);
	}

	const FLineOfSight LineOfSight(World, Box, TargetActors, SearchRadius + Box.GetExtent().Size());

	// 1. Nearest orientation with a fully clear view.
	int32 BestIndex = INDEX_NONE;
	int32 BestVisible = 0;

	for (int32 i = 0; i < Poses.Num(); ++i)
	{
		const int32 Visible = LineOfSight.CountVisibleSamples(Samples, Poses[i].Location);

		if (Visible == Samples.Num())
		{
			if (i > 0)
			{
				UE_LOG(LogAccession, Log, TEXT("ViewportCamera: Direct View Blocked, Moved Round By Yaw %.0f, Pitch %.0f."),
					FRotator::NormalizeAxis(Rotations[i].Yaw - Preferred.Yaw), Rotations[i].Pitch - Preferred.Pitch);
			}
			return Poses[i];
		}

		if (Visible > BestVisible)
		{
			BestVisible = Visible;
			BestIndex = i;
		}
	}

	// 2. No fully clear view: accept the best view where the centre and most of the item can be seen.
	if (BestIndex != INDEX_NONE && BestVisible * 2 >= Samples.Num())
	{
		UE_LOG(LogAccession, Log, TEXT("ViewportCamera: No Fully Clear View, Using Best (%d/%d Points Visible)."), BestVisible, Samples.Num());
		return Poses[BestIndex];
	}

	// 3. Enclosed (e.g. an item in a room seen from outside): pull the camera in front of the nearest blocker on the direct line.
	//    The item is then seen, but may be larger than the view.
	const double BlockingDistance = LineOfSight.GetBlockingDistance(DirectPose.Location);
	if (BlockingDistance > 0.0)
	{
		const double PulledDistance = BlockingDistance - OccluderMargin;
		if (PulledDistance > GetNearDistance(Box, Preferred))
		{
			UE_LOG(LogAccession, Log, TEXT("ViewportCamera: Item Enclosed, Camera Pulled In To %.0f From The Item."), PulledDistance);
			return MakePose(DirectPose, Centre, Preferred, PulledDistance);
		}
	}

	UE_LOG(LogAccession, Display, TEXT("ViewportCamera: No Clear View Of Item Found, Using Direct Framing."));
	return DirectPose;
}

void FViewportCameraController::Orbit(FEditorViewportClient* Client, float YawDegrees, float PitchDegrees)
{
	if (!Client)
		return;

	const UAccessionViewportSettings* Settings = GetDefault<UAccessionViewportSettings>();

	const FViewportCameraPose Current = GetIntendedPose(Client);

	if (!bLastActionWasOrbit)
	{
		PushHistory(Current);
	}
	bLastActionWasOrbit = true;

	// Orbit around whatever is centred in view, at the distance of the last look-at point.
	// After a focus this is the focused item; after manual flying it is still what the user is looking at.
	const double PivotDistance = FMath::Max(FVector::Dist(Current.Location, Current.LookAt), 100.0);
	const FVector Pivot = Current.Location + Current.Rotation.Vector() * PivotDistance;

	const FVector Offset = Current.Location - Pivot;
	FRotator OffsetRotation = Offset.Rotation();
	OffsetRotation.Yaw += YawDegrees;
	OffsetRotation.Pitch = FMath::Clamp<double>(OffsetRotation.Pitch + PitchDegrees, -Settings->MaxPitch, Settings->MaxPitch);

	FViewportCameraPose Target = Current;
	Target.Location = Pivot + OffsetRotation.Vector() * Offset.Size();
	Target.Rotation = (Pivot - Target.Location).Rotation();
	Target.Rotation.Roll = 0.0;
	Target.LookAt = Pivot;

	TransitionTo(Client, Target);
}

bool FViewportCameraController::Back(FEditorViewportClient* Client)
{
	if (!Client || History.IsEmpty())
		return false;

	const FViewportCameraPose Previous = History.Pop();
	bLastActionWasOrbit = false;

	TransitionTo(Client, Previous);
	return true;
}

void FViewportCameraController::Tick(FEditorViewportClient* Client, float DeltaTime)
{
	if (!bTransitioning || !Client)
		return;

	// The user moved the camera themselves (mouse, keyboard, another command): let go immediately.
	if (!FViewportCameraPose::Capture(Client).NearlyEquals(LastAppliedPose))
	{
		UE_LOG(LogAccession, Verbose, TEXT("ViewportCamera: Transition Cancelled By External Camera Input."));
		bTransitioning = false;
		return;
	}

	TransitionElapsed += DeltaTime;

	const float Linear = FMath::Clamp(TransitionElapsed / FMath::Max(TransitionDuration, UE_SMALL_NUMBER), 0.f, 1.f);
	const float Eased = FMath::SmoothStep(0.f, 1.f, Linear);

	LastAppliedPose = FViewportCameraPose::Blend(TransitionFrom, TransitionTarget, Eased);
	LastAppliedPose.Apply(Client);

	if (Linear >= 1.f)
	{
		bTransitioning = false;
	}
}

void FViewportCameraController::PushHistory(const FViewportCameraPose& Pose)
{
	if (!History.IsEmpty() && History.Last().NearlyEquals(Pose))
		return;

	History.Add(Pose);

	const int32 MaxHistory = GetDefault<UAccessionViewportSettings>()->MaxHistory;
	if (History.Num() > MaxHistory)
	{
		History.RemoveAt(0, History.Num() - MaxHistory);
	}
}

FViewportCameraPose FViewportCameraController::GetIntendedPose(const FEditorViewportClient* Client) const
{
	return bTransitioning ? TransitionTarget : FViewportCameraPose::Capture(Client);
}

void FViewportCameraController::TransitionTo(FEditorViewportClient* Client, const FViewportCameraPose& Target)
{
	const UAccessionViewportSettings* Settings = GetDefault<UAccessionViewportSettings>();

	if (Settings->bReducedMotion)
	{
		bTransitioning = false;
		Target.Apply(Client);
		return;
	}

	TransitionFrom = FViewportCameraPose::Capture(Client);
	TransitionTarget = Target;
	LastAppliedPose = TransitionFrom;
	TransitionElapsed = 0.f;
	TransitionDuration = Settings->TransitionSeconds;
	bTransitioning = true;
}