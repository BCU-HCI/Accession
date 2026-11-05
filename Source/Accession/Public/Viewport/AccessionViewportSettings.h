// Copyright (C) HCI-BCU 2025. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "AccessionViewportSettings.generated.h"

/**
 * Per-user settings for voice driven 3D viewport navigation.
 * Stored per user (EditorPerProjectUserSettings), as these are accessibility preferences, not project rules.
 */
UCLASS(config = EditorPerProjectUserSettings, meta = (DisplayName = "Accession Viewport"))
class ACCESSION_API UAccessionViewportSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	virtual FName GetCategoryName() const override { return TEXT("Plugins"); }

	// Labels

	/** Show index labels over items in the active viewport. Can be toggled with "VIEW NUMBERS". */
	UPROPERTY(config, EditAnywhere, Category = "Labels")
	bool bShowLabels = true;

	/** Maximum number of labels shown at once (largest items first). Numbers themselves are kept for the session. */
	UPROPERTY(config, EditAnywhere, Category = "Labels", meta = (ClampMin = "1", ClampMax = "99"))
	int32 MaxLabels = 24;

	/** Scale applied to label text and padding. */
	UPROPERTY(config, EditAnywhere, Category = "Labels", meta = (ClampMin = "0.5", ClampMax = "4.0"))
	float LabelScale = 1.25f;

	/** Items covering less than this fraction of the viewport are not indexed. */
	UPROPERTY(config, EditAnywhere, Category = "Labels", meta = (ClampMin = "0.0", ClampMax = "0.1"))
	float MinScreenCoverage = 0.0005f;

	/** Skip items whose centre and top are both hidden behind other geometry. */
	UPROPERTY(config, EditAnywhere, Category = "Labels")
	bool bOcclusionTest = true;

	/** Items with a bounds extent larger than this (sky spheres, landscapes, volumes) are not indexed. */
	UPROPERTY(config, EditAnywhere, Category = "Labels", meta = (ClampMin = "100.0"))
	float MaxTargetExtent = 50000.f;

	/** Seconds the camera must be still before labels are re-assigned. Prevents numbers changing mid-utterance. */
	UPROPERTY(config, EditAnywhere, Category = "Labels", meta = (ClampMin = "0.0", ClampMax = "3.0"))
	float SettleDelaySeconds = 0.35f;

	// Camera

	/** Cut instantly instead of animating camera moves (for motion sensitivity). */
	UPROPERTY(config, EditAnywhere, Category = "Camera")
	bool bReducedMotion = false;

	/** Duration of animated camera moves, in seconds. */
	UPROPERTY(config, EditAnywhere, Category = "Camera", meta = (ClampMin = "0.05", ClampMax = "3.0"))
	float TransitionSeconds = 0.6f;

	/** Space around a focused item: 1 fits its bounding box exactly to the view edges, 1.3 leaves roughly a 15% margin each side. */
	UPROPERTY(config, EditAnywhere, Category = "Camera", meta = (ClampMin = "1.0", ClampMax = "5.0"))
	float FramingPadding = 1.3f;

	/**
	 * When focusing, raycast between the item and the planned camera position (both directions, Visibility channel),
	 * and if something is in the way move round to the nearest clear angle, or in front of the blocker.
	 */
	UPROPERTY(config, EditAnywhere, Category = "Camera")
	bool bEnsureLineOfSight = true;

	/** Also treat meshes without Visibility collision (e.g. foliage) as blocking, using their bounds. */
	UPROPERTY(config, EditAnywhere, Category = "Camera", meta = (EditCondition = "bEnsureLineOfSight"))
	bool bMeshBoundsOcclusion = true;

	/** Pitch, in degrees, a focus starts from (negative looks down). Keeps repeated focuses at a steady eye level. */
	UPROPERTY(config, EditAnywhere, Category = "Camera", meta = (ClampMin = "-60.0", ClampMax = "20.0"))
	float FocusPitch = -20.f;

	/** How far, in degrees, the current camera pitch may differ from Focus Pitch before a focus brings it back. */
	UPROPERTY(config, EditAnywhere, Category = "Camera", meta = (ClampMin = "0.0", ClampMax = "45.0"))
	float FocusPitchTolerance = 15.f;

	/** Select the focused item in the level (selection outline and Details panel), so it is highlighted as well as framed. */
	UPROPERTY(config, EditAnywhere, Category = "Camera")
	bool bSelectOnFocus = true;

	/** Number of camera poses kept for "VIEW BACK". */
	UPROPERTY(config, EditAnywhere, Category = "Camera", meta = (ClampMin = "1", ClampMax = "100"))
	int32 MaxHistory = 20;

	/** Pitch limit, in degrees, applied when focusing and orbiting so the camera never flips over the top. */
	UPROPERTY(config, EditAnywhere, Category = "Camera", meta = (ClampMin = "10.0", ClampMax = "89.0"))
	float MaxPitch = 80.f;
};