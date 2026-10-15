// Copyright (C) HCI-BCU 2025. All rights reserved.

#include "Viewport/ViewportCameraController.h"
#include "Viewport/AccessionViewportSettings.h"
#include "AccessionLogging.h"

#include "EditorViewportClient.h"

// FViewportCameraPose

FViewportCameraPose FViewportCameraPose::Capture(const FEditorViewportClient *Client)
{
	FViewportCameraPose Pose;
	Pose.Location = Client->GetViewLocation();
	Pose.Rotation = Client->GetViewRotation();
	Pose.LookAt = Client->GetLookAtLocation();
	Pose.OrthoZoom = Client->GetOrthoZoom();
	return Pose;
}

void FViewportCameraPose::Apply(FEditorViewportClient *Client) const
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

bool FViewportCameraPose::NearlyEquals(const FViewportCameraPose &Other, double LocationTolerance, double RotationTolerance) const
{
	return Location.Equals(Other.Location, LocationTolerance) &&
		   Rotation.Equals(Other.Rotation, RotationTolerance) &&
		   FMath::IsNearlyEqual(OrthoZoom, Other.OrthoZoom, 1.f);
}

FViewportCameraPose FViewportCameraPose::Blend(const FViewportCameraPose &From, const FViewportCameraPose &To, float Alpha)
{
	FViewportCameraPose Out;
	Out.Location = FMath::Lerp(From.Location, To.Location, Alpha);
	Out.LookAt = FMath::Lerp(From.LookAt, To.LookAt, Alpha);
	Out.Rotation = FQuat::Slerp(From.Rotation.Quaternion(), To.Rotation.Quaternion(), Alpha).Rotator();
	Out.OrthoZoom = FMath::Lerp(From.OrthoZoom, To.OrthoZoom, Alpha);
	return Out;
}

// FViewportCameraController

void FViewportCameraController::FocusOnBox(FEditorViewportClient *Client, const FBox &Box)
{
	if (!Client || !Box.IsValid)
		return;

	const UAccessionViewportSettings *Settings = GetDefault<UAccessionViewportSettings>();

	PushHistory(GetIntendedPose(Client));
	bLastActionWasOrbit = false;

	if (Client->IsOrtho())
	{
		// The engine's own framing already handles orthographic zoom correctly.
		bTransitioning = false;
		Client->FocusViewportOnBox(Box, /*bInstant*/ Settings->bReducedMotion);
		return;
	}

	const FViewportCameraPose Current = GetIntendedPose(Client);
	const FVector Centre = Box.GetCenter();

	// Bounding sphere radius, so the whole item fits from any angle.
	const double Radius = FMath::Max(Box.GetExtent().Size(), 10.0);

	// Fit the sphere to the narrower of the horizontal / vertical field of view.
	const FIntPoint ViewSize = Client->Viewport ? Client->Viewport->GetSizeXY() : FIntPoint(16, 9);
	const double Aspect = ViewSize.Y > 0 ? double(ViewSize.X) / double(ViewSize.Y) : 16.0 / 9.0;
	const double HalfHorizontal = FMath::DegreesToRadians(FMath::Clamp<double>(Client->ViewFOV, 5.0, 170.0) * 0.5);
	const double HalfVertical = FMath::Atan(FMath::Tan(HalfHorizontal) / Aspect);
	const double HalfFOV = FMath::Min(HalfHorizontal, HalfVertical);

	const double Distance = FMath::Max(Radius / FMath::Sin(HalfFOV) * Settings->FramingPadding, Radius + 10.0);

	// Approach along the current line of sight to the item. If the camera is inside it, keep facing the same way.
	FVector ToTarget = Centre - Current.Location;
	FRotator Rotation = ToTarget.IsNearlyZero(1.0) ? Current.Rotation : ToTarget.Rotation();
	Rotation.Pitch = FMath::Clamp<double>(Rotation.Pitch, -Settings->MaxPitch, Settings->MaxPitch);
	Rotation.Roll = 0.0;

	FViewportCameraPose Target = Current;
	Target.Rotation = Rotation;
	Target.Location = Centre - Rotation.Vector() * Distance;
	Target.LookAt = Centre;

	TransitionTo(Client, Target);
}

void FViewportCameraController::Orbit(FEditorViewportClient *Client, float YawDegrees, float PitchDegrees)
{
	if (!Client)
		return;

	const UAccessionViewportSettings *Settings = GetDefault<UAccessionViewportSettings>();

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

bool FViewportCameraController::Back(FEditorViewportClient *Client)
{
	if (!Client || History.IsEmpty())
		return false;

	const FViewportCameraPose Previous = History.Pop();
	bLastActionWasOrbit = false;

	TransitionTo(Client, Previous);
	return true;
}

void FViewportCameraController::Tick(FEditorViewportClient *Client, float DeltaTime)
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

void FViewportCameraController::PushHistory(const FViewportCameraPose &Pose)
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

FViewportCameraPose FViewportCameraController::GetIntendedPose(const FEditorViewportClient *Client) const
{
	return bTransitioning ? TransitionTarget : FViewportCameraPose::Capture(Client);
}

void FViewportCameraController::TransitionTo(FEditorViewportClient *Client, const FViewportCameraPose &Target)
{
	const UAccessionViewportSettings *Settings = GetDefault<UAccessionViewportSettings>();

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
