// Copyright (C) HCI-BCU 2025. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectKey.h"

class FEditorViewportClient;
class AActor;
class UPrimitiveComponent;

/**
 * Something in a viewport that can be indexed and focused.
 * Level viewports index actors; asset editor (preview) viewports index primitive components.
 */
struct ACCESSION_API FViewportTarget
{
	TWeakObjectPtr<AActor> Actor;
	TWeakObjectPtr<UPrimitiveComponent> Component;

	bool IsValid() const;

	/** World space bounds of the target. Invalid box if the target has gone. */
	FBox GetBounds() const;

	/** Display name, for logging and future spoken / on screen feedback. */
	FString GetLabel() const;

	FObjectKey GetKey() const;
};

/**
 * Gives each item in a world a number that it keeps for the editor session, like the graph indexer does for nodes.
 *
 *  - An item is numbered the first time it is seen, and keeps that number when it leaves the view and comes back.
 *  - Only deleting the item releases its number; the smallest free number is reused for the next new item.
 *  - Refresh() only changes which numbers are visible (labelled) right now, never which item a number points to.
 *
 * One indexer is shared by every viewport showing the same world, so all panes agree on the numbers.
 */
class ACCESSION_API FViewportTargetIndexer
{
public:
	/**
	 * Updates which items are visible in the provided viewport, numbering any seen for the first time.
	 * @param Client A valid viewport client (validated by the caller).
	 */
	void Refresh(FEditorViewportClient* Client);

	/** Forgets every number, so numbering starts again from 0. */
	void Reset();

	/** @return The target for the index (visible or not), or nullptr if the index is unused or the target has gone. */
	const FViewportTarget* Find(int32 Index) const;

	/** @return The index of the target, or INDEX_NONE. */
	int32 FindIndex(const FObjectKey& TargetKey) const;

	/** @return All indexes in ascending order, including items not currently in view. */
	TArray<int32> GetSortedIndexes() const;

	/** @return Indexes visible (and labelled) in the last refreshed viewport, in ascending order. */
	const TArray<int32>& GetVisibleIndexes() const { return VisibleIndexes; }

	/** @return A readable table of every index, for logging. */
	FString Describe() const;

	const TMap<int32, FViewportTarget>& GetEntries() const { return IndexToTarget; }

	int32 Num() const { return IndexToTarget.Num(); }

	/** First index handed out. Matches the graph indexer, which counts from 0. */
	static constexpr int32 FirstIndex = 0;

private:
	struct FCandidate
	{
		FViewportTarget Target;
		float ScreenCoverage = 0.f;
	};

	void GatherCandidates(FEditorViewportClient* Client, TArray<FCandidate>& OutCandidates) const;

	void PruneDestroyed();

	/** Pops the smallest freed index if there is one, otherwise climbs to the next unused index. */
	int32 TakeIndex();

	TMap<int32, FViewportTarget> IndexToTarget;

	/** Indexes released by deleted items, kept as a min-heap so the smallest is reused first. */
	TArray<int32> FreedIndexes;

	/** The next never-used index, once FreedIndexes is empty. */
	int32 NextIndex = FirstIndex;

	TMap<FObjectKey, int32> TargetToIndex;
	TArray<int32> VisibleIndexes;
};