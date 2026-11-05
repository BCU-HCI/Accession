// Copyright (C) HCI-BCU 2025. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "UObject/ObjectKey.h"

#include "Viewport/ViewportTargetIndexer.h"
#include "Viewport/ViewportCameraController.h"

class FEditorViewportClient;
class UCanvas;
class IConsoleObject;
class APlayerController;

/**
 * Entry point for voice driven 3D viewport navigation.
 *
 * Owns a target indexer per world (the numbers drawn over items, kept for the session and shared by every pane
 * showing that world) and a camera controller per editor viewport.
 * Labels are drawn only in the viewport that voice commands currently act on, so there is one set of numbers on screen.
 *
 * Owned by FAccessionModule, alongside the AssetRegistry.
 */
class ACCESSION_API FAccessionViewportNavigator
{
public:
	FAccessionViewportNavigator() = default;
	~FAccessionViewportNavigator() = default;

	// Non-copyable. States holds TUniquePtrs, and MSVC generates every special member of an exported (ACCESSION_API)
	// class, so an implicit copy constructor would fail to compile (TSparseSetElement deleted function).
	FAccessionViewportNavigator(const FAccessionViewportNavigator&) = delete;
	FAccessionViewportNavigator& operator=(const FAccessionViewportNavigator&) = delete;

	void Initialize();
	void Shutdown();

	/**
	 * Finds the viewport that voice commands should act on.
	 * Prefers the viewport inside the active tab (choosing the focused pane in multi-pane layouts),
	 * then falls back to the last used level viewport. Returns nullptr when a graph editor is active,
	 * so 3D commands never move a camera the user cannot see.
	 */
	static FEditorViewportClient* ResolveActiveViewportClient();

	/** @return True if the client still exists. FEditorViewportClient is not a UObject, so raw pointers must be checked. */
	static bool IsClientAlive(const FEditorViewportClient* Client);

	// Commands. Each returns false (and logs why) when nothing could be done.

	/** Frames the item with the given number. */
	bool FocusIndex(int32 Index);

	/** Frames the next (+1) or previous (-1) visible numbered item, relative to the last focused one, wrapping around. */
	bool FocusStep(int32 Step);

	/** Returns the camera to where it was before the last navigation step. */
	bool Back();

	/** Orbits around the item in the centre of view. */
	bool Orbit(float YawDegrees, float PitchDegrees);

	/** Frames the selected actors, or every visible numbered item when nothing is selected. */
	bool FrameSelection();

	/** Shows or hides the number labels. */
	void ToggleLabels();

	/** Stops any running camera transition on the client, so manual moves take over cleanly. */
	void CancelTransition(FEditorViewportClient* Client);

	/** Writes every number in the active viewport's world, and what it points to, to the output log. */
	void LogIndexes();

	/** Forgets every number in the active viewport's world, so numbering starts again from 0. */
	void ResetIndexes();

	/** Makes the target the only selected actor, when the client shows the level being edited. Undoable. */
	void SelectTarget(FEditorViewportClient* Client, const FViewportTarget& Target);

private:
	struct FViewportState
	{
		FViewportCameraController Camera;

		FViewportCameraPose LastObservedPose;
		double LastMovedTime = 0.0;
		double LastRefreshTime = 0.0;
		bool bNeedsRefresh = true;

		int32 FocusedIndex = INDEX_NONE;
	};

	FViewportState& GetOrAddState(FEditorViewportClient* Client);

	/** The indexer for the world the client shows, created on first use. Null if the client has no world. */
	FViewportTargetIndexer* GetIndexer(FEditorViewportClient* Client);

	/** Resolves the active client and makes sure its world has been indexed at least once. */
	FEditorViewportClient* GetCommandClient(FViewportState*& OutState, FViewportTargetIndexer*& OutIndexer);

	bool Tick(float DeltaTime);

	void UpdateIndexing(FEditorViewportClient* Client, FViewportState& State);

	void PruneDeadClients();

	void SetLabelledClient(FEditorViewportClient* NewClient);

	void DrawLabels(UCanvas* Canvas, APlayerController* PlayerController);

	TMap<FEditorViewportClient*, TUniquePtr<FViewportState>> States;

	/** Numbering per world. Keyed weakly, so a closed level or asset editor drops its numbers. */
	TMap<FObjectKey, TUniquePtr<FViewportTargetIndexer>> WorldIndexers;

	TArray<IConsoleObject*> ConsoleCommands;

	/** The viewport currently showing labels (the one voice commands act on). */
	FEditorViewportClient* LabelledClient = nullptr;

	bool bLabelsVisible = true;

	float ResolveAccumulator = 0.f;

	FTSTicker::FDelegateHandle TickHandle;
	FDelegateHandle DrawHandle;
};