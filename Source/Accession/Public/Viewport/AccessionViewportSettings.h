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

	/** Maximum number of items indexed at once. Fewer labels means less clutter and shorter numbers to say. */
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

	/** Multiplier on the tight framing distance; above 1 leaves space around the focused item. */
	UPROPERTY(config, EditAnywhere, Category = "Camera", meta = (ClampMin = "1.0", ClampMax = "5.0"))
	float FramingPadding = 1.3f;

	/** Number of camera poses kept for "VIEW BACK". */
	UPROPERTY(config, EditAnywhere, Category = "Camera", meta = (ClampMin = "1", ClampMax = "100"))
	int32 MaxHistory = 20;

	/** Pitch limit, in degrees, applied when focusing and orbiting so the camera never flips over the top. */
	UPROPERTY(config, EditAnywhere, Category = "Camera", meta = (ClampMin = "10.0", ClampMax = "89.0"))
	float MaxPitch = 80.f;
};
