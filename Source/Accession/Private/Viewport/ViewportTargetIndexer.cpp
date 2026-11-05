// Copyright (C) HCI-BCU 2025. All rights reserved.

#include "Viewport/ViewportTargetIndexer.h"
#include "Viewport/AccessionViewportSettings.h"
#include "AccessionLogging.h"

#include "EditorViewportClient.h"
#include "EngineUtils.h"
#include "SceneView.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/WorldSettings.h"
#include "Engine/Level.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"

// FViewportTarget

bool FViewportTarget::IsValid() const
{
	return Component.IsValid() || Actor.IsValid();
}

FBox FViewportTarget::GetBounds() const
{
	if (const UPrimitiveComponent* Comp = Component.Get())
	{
		return Comp->Bounds.GetBox();
	}

	if (const AActor* OwnerActor = Actor.Get())
	{
		const FBox Box = OwnerActor->GetComponentsBoundingBox(/*bNonColliding*/ true, /*bIncludeFromChildActors*/ true);
		if (Box.IsValid)
		{
			return Box;
		}

		// Actors without primitive bounds (e.g. some lights, empty actors) still get a small box at their pivot.
		return FBox::BuildAABB(OwnerActor->GetActorLocation(), FVector(50.0));
	}

	return FBox(ForceInit);
}

FString FViewportTarget::GetLabel() const
{
	if (const UPrimitiveComponent* Comp = Component.Get())
	{
		return Comp->GetName();
	}

	if (const AActor* OwnerActor = Actor.Get())
	{
		return OwnerActor->GetActorLabel();
	}

	return TEXT("<Invalid>");
}

FObjectKey FViewportTarget::GetKey() const
{
	if (Component.IsValid())
	{
		return FObjectKey(Component.Get());
	}

	return FObjectKey(Actor.Get());
}

// Helpers

namespace ViewportIndexerUtils
{
	/** Projects a world point to viewport pixels. Returns false when the point is behind the camera. */
	static bool ProjectToPixel(const FSceneView& View, const FVector& WorldPoint, FVector2D& OutPixel)
	{
		const FVector4 ScreenPoint = View.WorldToScreen(WorldPoint);
		if (ScreenPoint.W <= UE_KINDA_SMALL_NUMBER)
		{
			return false;
		}

		return View.ScreenToPixel(ScreenPoint, OutPixel);
	}

	/** True if a trace from the camera to the sample point reaches the target before anything else. */
	static bool IsSampleVisible(UWorld* World, const FVector& From, const FVector& To, const FViewportTarget& Target)
	{
		FCollisionQueryParams Params(SCENE_QUERY_STAT(AccessionViewportOcclusion), /*bTraceComplex*/ true);

		FHitResult Hit;
		if (!World->LineTraceSingleByChannel(Hit, From, To, ECC_Visibility, Params))
		{
			// Nothing in the way (or the target has no collision).
			return true;
		}

		const AActor* HitActor = Hit.GetActor();
		if (Target.Component.IsValid())
		{
			return Hit.GetComponent() == Target.Component.Get() || HitActor == Target.Component->GetOwner();
		}

		return HitActor == Target.Actor.Get();
	}
}

// FViewportTargetIndexer

void FViewportTargetIndexer::Refresh(FEditorViewportClient* Client)
{
	// Only deleted items give their number back. Leaving the view never does.
	PruneDestroyed();

	TArray<FCandidate> Candidates;
	GatherCandidates(Client, Candidates);

	// Largest items get the label slots (and, when new, the smallest free numbers).
	Candidates.Sort([](const FCandidate& A, const FCandidate& B)
		{ return A.ScreenCoverage > B.ScreenCoverage; });

	const int32 MaxLabels = GetDefault<UAccessionViewportSettings>()->MaxLabels;
	if (Candidates.Num() > MaxLabels)
	{
		Candidates.SetNum(MaxLabels);
	}

	VisibleIndexes.Reset();

	for (const FCandidate& Candidate : Candidates)
	{
		const FObjectKey Key = Candidate.Target.GetKey();

		int32 Index;
		if (const int32* Existing = TargetToIndex.Find(Key))
		{
			Index = *Existing;
		}
		else
		{
			Index = TakeIndex();
			IndexToTarget.Add(Index, Candidate.Target);
			TargetToIndex.Add(Key, Index);

			UE_LOG(LogAccession, Log, TEXT("ViewportTargetIndexer: %d -> %s"), Index, *Candidate.Target.GetLabel());
		}

		VisibleIndexes.Add(Index);
	}

	VisibleIndexes.Sort();
}

void FViewportTargetIndexer::PruneDestroyed()
{
	TSet<int32> Released;
	for (auto It = IndexToTarget.CreateIterator(); It; ++It)
	{
		if (!It.Value().IsValid())
		{
			UE_LOG(LogAccession, Log, TEXT("ViewportTargetIndexer: %d Released (Item Deleted)."), It.Key());

			Released.Add(It.Key());
			FreedIndexes.HeapPush(It.Key());
			It.RemoveCurrent();
		}
	}

	if (Released.IsEmpty())
		return;

	// The target is gone, so its key can't be rebuilt from it: remove the reverse entry by index instead.
	for (auto It = TargetToIndex.CreateIterator(); It; ++It)
	{
		if (Released.Contains(It.Value()))
		{
			It.RemoveCurrent();
		}
	}

	VisibleIndexes.RemoveAll([&Released](int32 Index)
		{ return Released.Contains(Index); });
}

void FViewportTargetIndexer::Reset()
{
	IndexToTarget.Reset();
	TargetToIndex.Reset();
	VisibleIndexes.Reset();
	FreedIndexes.Reset();
	NextIndex = FirstIndex;
}

FString FViewportTargetIndexer::Describe() const
{
	FString Out;
	for (const int32 Index : GetSortedIndexes())
	{
		const FViewportTarget& Target = IndexToTarget[Index];
		Out += FString::Printf(TEXT("  %3d %s %s\n"),
			Index,
			VisibleIndexes.Contains(Index) ? TEXT("[visible]") : TEXT("         "),
			*Target.GetLabel());
	}
	return Out;
}

const FViewportTarget* FViewportTargetIndexer::Find(int32 Index) const
{
	const FViewportTarget* Found = IndexToTarget.Find(Index);
	return (Found && Found->IsValid()) ? Found : nullptr;
}

int32 FViewportTargetIndexer::FindIndex(const FObjectKey& TargetKey) const
{
	const int32* Found = TargetToIndex.Find(TargetKey);
	return Found ? *Found : INDEX_NONE;
}

TArray<int32> FViewportTargetIndexer::GetSortedIndexes() const
{
	TArray<int32> Indexes;
	IndexToTarget.GetKeys(Indexes);
	Indexes.Sort();
	return Indexes;
}

int32 FViewportTargetIndexer::TakeIndex()
{
	if (!FreedIndexes.IsEmpty())
	{
		int32 Index;
		FreedIndexes.HeapPop(Index);
		return Index;
	}

	return NextIndex++;
}

void FViewportTargetIndexer::GatherCandidates(FEditorViewportClient* Client, TArray<FCandidate>& OutCandidates) const
{
	UWorld* World = Client ? Client->GetWorld() : nullptr;
	if (!World || !Client->Viewport || Client->Viewport->GetSizeXY().GetMin() <= 0)
	{
		return;
	}

	const UAccessionViewportSettings* Settings = GetDefault<UAccessionViewportSettings>();

	// Build a scene view matching what the user currently sees, for frustum tests and projection.
	FSceneViewFamilyContext ViewFamily(
		FSceneViewFamily::ConstructionValues(Client->Viewport, Client->GetScene(), Client->EngineShowFlags)
		.SetRealtimeUpdate(Client->IsRealtime()));

	const FSceneView* View = Client->CalcSceneView(&ViewFamily);
	if (!View)
	{
		return;
	}

	const FIntRect ViewRect = View->UnscaledViewRect;
	const double ViewArea = FMath::Max<double>(1.0, double(ViewRect.Width()) * double(ViewRect.Height()));
	const FVector ViewOrigin = View->ViewMatrices.GetViewOrigin();

	auto Consider = [&](const FViewportTarget& Target)
		{
			const FBox Bounds = Target.GetBounds();
			if (!Bounds.IsValid)
				return;

			FVector Centre, Extent;
			Bounds.GetCenterAndExtents(Centre, Extent);

			if (Extent.GetMax() > Settings->MaxTargetExtent)
				return;

			if (!View->ViewFrustum.IntersectBox(Centre, Extent))
				return;

			// The label sits on the centre of the item, so the centre has to be on screen.
			FVector2D CentrePixel;
			if (!ViewportIndexerUtils::ProjectToPixel(*View, Centre, CentrePixel) ||
				!FBox2D(FVector2D(ViewRect.Min), FVector2D(ViewRect.Max)).IsInside(CentrePixel))
				return;

			// Approximate on-screen size from the projected bounds corners.
			FBox2D ScreenBox(ForceInit);
			for (int32 Corner = 0; Corner < 8; ++Corner)
			{
				const FVector CornerPoint = Centre + Extent * FVector(
					(Corner & 1) ? 1.0 : -1.0,
					(Corner & 2) ? 1.0 : -1.0,
					(Corner & 4) ? 1.0 : -1.0);

				FVector2D CornerPixel;
				if (ViewportIndexerUtils::ProjectToPixel(*View, CornerPoint, CornerPixel))
				{
					ScreenBox += CornerPixel;
				}
			}

			if (!ScreenBox.bIsValid)
				return;

			ScreenBox.Min = ScreenBox.Min.ComponentMax(FVector2D(ViewRect.Min));
			ScreenBox.Max = ScreenBox.Max.ComponentMin(FVector2D(ViewRect.Max));

			const FVector2D ScreenSize = (ScreenBox.Max - ScreenBox.Min).ComponentMax(FVector2D::ZeroVector);
			const float Coverage = float((ScreenSize.X * ScreenSize.Y) / ViewArea);
			if (Coverage < Settings->MinScreenCoverage)
				return;

			if (Settings->bOcclusionTest && !Client->IsOrtho())
			{
				const FVector TopCentre = Centre + FVector(0.0, 0.0, Extent.Z * 0.9);

				if (!ViewportIndexerUtils::IsSampleVisible(World, ViewOrigin, Centre, Target) &&
					!ViewportIndexerUtils::IsSampleVisible(World, ViewOrigin, TopCentre, Target))
					return;
			}

			FCandidate& Candidate = OutCandidates.AddDefaulted_GetRef();
			Candidate.Target = Target;
			Candidate.ScreenCoverage = Coverage;
		};

	const bool bPreviewWorld = World->WorldType == EWorldType::EditorPreview;

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!::IsValid(Actor) || Actor->IsA<AWorldSettings>())
			continue;

		if (bPreviewWorld)
		{
			// Asset editors usually show a single preview actor, so its components are the useful targets.
			Actor->ForEachComponent<UPrimitiveComponent>(false, [&](UPrimitiveComponent* Component)
				{
					if (!Component->IsRegistered() || !Component->IsVisible() || Component->IsVisualizationComponent())
						return;

					FViewportTarget Target;
					Target.Actor = Actor;
					Target.Component = Component;
					Consider(Target); });

			continue;
		}

		if (Actor->IsHiddenEd() || !Actor->IsListedInSceneOutliner())
			continue;

		if (const ULevel* Level = Actor->GetLevel(); Level && !Level->bIsVisible)
			continue;

		FViewportTarget Target;
		Target.Actor = Actor;
		Consider(Target);
	}
}