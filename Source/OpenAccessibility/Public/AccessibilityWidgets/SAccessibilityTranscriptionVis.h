// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/AppStyle.h"
#include "Widgets/Layout/SBorder.h"

class OPENACCESSIBILITY_API SAccessibilityTranscriptionVis : public SOverlay
{
public:

	SLATE_BEGIN_ARGS(SAccessibilityTranscriptionVis)
		{}
		SLATE_ARGUMENT( int, VisAmount )
	SLATE_END_ARGS()

	~SAccessibilityTranscriptionVis();

	void Construct(const FArguments& InArgs);

	// SWidget Interface

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	// End of SWidget Interface

protected:


};