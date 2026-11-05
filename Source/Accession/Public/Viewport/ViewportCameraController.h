// Copyright (C) HCI-BCU 2025. All rights reserved.

#pragma once

#include "CoreMinimal.h"

class FEditorViewportClient;
class AActor;
class UWorld;

namespace ViewportFraming
{
	struct FLens;
}

/**
 * A snapshot of an editor viewport camera.
 */
struct ACCESSION_API FViewportCameraPose
{
	FVector Location = FVector::ZeroVector;
	FRotator Rotation = FRotator::ZeroRotator;
	FVector LookAt = FVector::ZeroVector;
	float OrthoZoom = 0.f;

	static FViewportCameraPose Capture(const FEditorViewportClient* Client);

	void Apply(FEditorViewportClient* Client) const;

	bool NearlyEquals(const FViewportCameraPose& Other, double LocationTolerance = 0.5, double RotationTolerance = 0.05) const;

	static FViewportCameraPose Blend(const FViewportCameraPose& From, const FViewportCameraPose& To, float Alpha);
};

/**
 * Camera wrapper for one editor viewport: framing, eased transitions, orbiting and history.
 *
 * Design notes (accessibility):
 *  - Focusing frames the item's bounding box tightly and moves along the line from where the camera is to the item,
 *    so the user keeps their sense of direction. If that view is blocked, it moves round to the nearest clear angle.
 *  - Moves are animated with an ease-in/out curve, or cut instantly when reduced motion is enabled.
 *  - Every jump is recorded, so "VIEW BACK" can always undo a navigation step.
 *  - Any manual camera input during a transition cancels it, the user is never fought for control.
 *
 * The controller does not hold the viewport client; callers pass a client they have validated,
 * as FEditorViewportClient is not a UObject and can be destroyed when a tab closes.
 */
class ACCESSION_API FViewportCameraController
{
public:
	/**
	 * Frames the box so all of it fits in the view, moving towards it from the current camera position.
	 * When line of sight is enabled, checks the item can actually be seen from there (raycasts against the Visibility channel),
	 * and if something is in the way moves round to the nearest clear angle instead.
	 * @param TargetActors The actors being framed; ignored by the line of sight traces so they never block themselves.
	 */
	void FocusOnBox(FEditorViewportClient* Client, const FBox& Box, const TArray<const AActor*>& TargetActors = TArray<const AActor*>());

	/** Orbits around the point the camera is centred on, by the given degrees. Positive yaw orbits left, positive pitch orbits up. */
	void Orbit(FEditorViewportClient* Client, float YawDegrees, float PitchDegrees);

	/** Returns to the pose before the last navigation step. @return False if there is no history. */
	bool Back(FEditorViewportClient* Client);

	/** Advances any running transition. */
	void Tick(FEditorViewportClient* Client, float DeltaTime);

	/** Stops any running transition where it is. */
	void CancelTransition() { bTransitioning = false; }

	bool IsTransitioning() const { return bTransitioning; }

	void ClearHistory() { History.Reset(); }

private:
	void PushHistory(const FViewportCameraPose& Pose);

	/**
	 * Searches for a framing of the box with a clear line of sight, trying orientations closest to Preferred first.
	 * @return A fully clear framing; else the best partly clear one; else the camera pulled in front of the occluder; else DirectPose.
	 */
	FViewportCameraPose FindClearFraming(
		UWorld* World,
		const FBox& Box,
		const FRotator& Preferred,
		const ViewportFraming::FLens& Lens,
		const TArray<const AActor*>& TargetActors,
		const FViewportCameraPose& DirectPose) const;

	/** The pose the camera is at, or is heading towards if a transition is running. */
	FViewportCameraPose GetIntendedPose(const FEditorViewportClient* Client) const;

	void TransitionTo(FEditorViewportClient* Client, const FViewportCameraPose& Target);

	TArray<FViewportCameraPose> History;

	bool bTransitioning = false;
	FViewportCameraPose TransitionFrom;
	FViewportCameraPose TransitionTarget;
	FViewportCameraPose LastAppliedPose;
	float TransitionElapsed = 0.f;
	float TransitionDuration = 0.f;

	/** Consecutive orbit steps share one history entry, so "BACK" undoes the whole orbit. */
	bool bLastActionWasOrbit = false;
};