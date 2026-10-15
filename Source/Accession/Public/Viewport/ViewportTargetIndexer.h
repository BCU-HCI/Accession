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
 * Assigns small, stable numbers to the visible items of one editor viewport.
 *
 * Stability rules (so a number does not change between the user reading it and saying it):
 *  - Numbers are only re-assigned on Refresh(), which the navigator calls once the camera has settled.
 *  - An item that stays visible keeps its number.
 *  - New items take the smallest free number, keeping spoken numbers short.
 *  - Items that leave the view release their number.
 */
class ACCESSION_API FViewportTargetIndexer
{
public:
	/**
	 * Re-gathers the visible items for the provided viewport and updates the index.
	 * @param Client A valid viewport client (validated by the caller).
	 */
	void Refresh(FEditorViewportClient *Client);

	/** Removes all indexes. */
	void Reset();

	/** @return The target for the index, or nullptr if the index is unused or the target has gone. */
	const FViewportTarget *Find(int32 Index) const;

	/** @return The index of the target, or INDEX_NONE. */
	int32 FindIndex(const FObjectKey &TargetKey) const;

	/** @return All indexes in ascending order. */
	TArray<int32> GetSortedIndexes() const;

	const TMap<int32, FViewportTarget> &GetEntries() const { return IndexToTarget; }

	int32 Num() const { return IndexToTarget.Num(); }

	/** First index handed out. Matches the graph indexer, which counts from 0. */
	static constexpr int32 FirstIndex = 0;

private:
	struct FCandidate
	{
		FViewportTarget Target;
		float ScreenCoverage = 0.f;
		bool bPreviouslyIndexed = false;
	};

	void GatherCandidates(FEditorViewportClient *Client, TArray<FCandidate> &OutCandidates) const;

	int32 TakeSmallestFreeIndex() const;

	TMap<int32, FViewportTarget> IndexToTarget;
	TMap<FObjectKey, int32> TargetToIndex;
};
