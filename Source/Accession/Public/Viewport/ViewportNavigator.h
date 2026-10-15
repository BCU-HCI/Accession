// Copyright (C) HCI-BCU 2025. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Ticker.h"

#include "Viewport/ViewportTargetIndexer.h"
#include "Viewport/ViewportCameraController.h"

class FEditorViewportClient;
class UCanvas;
class APlayerController;

/**
 * Entry point for voice driven 3D viewport navigation.
 *
 * Owns, per editor viewport: a target indexer (the numbers drawn over items) and a camera controller.
 * Labels are drawn only in the viewport that voice commands currently act on, so there is one set of numbers on screen.
 *
 * Owned by FAccessionModule, alongside the AssetRegistry.
 */
class ACCESSION_API FAccessionViewportNavigator
{
public:
	void Initialize();
	void Shutdown();

	/**
	 * Finds the viewport that voice commands should act on.
	 * Prefers the viewport inside the active tab (choosing the focused pane in multi-pane layouts),
	 * then falls back to the last used level viewport. Returns nullptr when a graph editor is active,
	 * so 3D commands never move a camera the user cannot see.
	 */
	static FEditorViewportClient *ResolveActiveViewportClient();

	/** @return True if the client still exists. FEditorViewportClient is not a UObject, so raw pointers must be checked. */
	static bool IsClientAlive(const FEditorViewportClient *Client);

	// Commands. Each returns false (and logs why) when nothing could be done.

	/** Frames the item with the given number. */
	bool FocusIndex(int32 Index);

	/** Frames the next (+1) or previous (-1) numbered item, relative to the last focused one, wrapping around. */
	bool FocusStep(int32 Step);

	/** Returns the camera to where it was before the last navigation step. */
	bool Back();

	/** Orbits around the item in the centre of view. */
	bool Orbit(float YawDegrees, float PitchDegrees);

	/** Frames the selected actors, or every numbered item when nothing is selected. */
	bool FrameSelection();

	/** Shows or hides the number labels. */
	void ToggleLabels();

	/** Stops any running camera transition on the client, so manual moves take over cleanly. */
	void CancelTransition(FEditorViewportClient *Client);

private:
	struct FViewportState
	{
		FViewportTargetIndexer Indexer;
		FViewportCameraController Camera;

		FViewportCameraPose LastObservedPose;
		double LastMovedTime = 0.0;
		double LastRefreshTime = 0.0;
		bool bNeedsRefresh = true;

		int32 FocusedIndex = INDEX_NONE;
	};

	FViewportState &GetOrAddState(FEditorViewportClient *Client);

	/** Resolves the active client and makes sure it has been indexed at least once. */
	FEditorViewportClient *GetCommandClient(FViewportState *&OutState);

	bool Tick(float DeltaTime);

	void UpdateIndexing(FEditorViewportClient *Client, FViewportState &State);

	void PruneDeadClients();

	void SetLabelledClient(FEditorViewportClient *NewClient);

	void DrawLabels(UCanvas *Canvas, APlayerController *PlayerController);

	TMap<FEditorViewportClient *, TUniquePtr<FViewportState>> States;

	/** The viewport currently showing labels (the one voice commands act on). */
	FEditorViewportClient *LabelledClient = nullptr;

	bool bLabelsVisible = true;

	float ResolveAccumulator = 0.f;

	FTSTicker::FDelegateHandle TickHandle;
	FDelegateHandle DrawHandle;
};
