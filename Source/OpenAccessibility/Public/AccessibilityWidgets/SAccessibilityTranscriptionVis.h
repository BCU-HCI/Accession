// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/AppStyle.h"
#include "Widgets/Layout/SBorder.h"

class OPENACCESSIBILITY_API SAccessibilityTranscriptionVis : public SBox
{
public:

	SLATE_BEGIN_ARGS(SAccessibilityTranscriptionVis)
	: _VisAmount(1)
	{}
		SLATE_ARGUMENT( int, VisAmount )
	SLATE_END_ARGS()

	~SAccessibilityTranscriptionVis();

	void Construct(const FArguments& InArgs);

	// SWidget Interface

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	// End of SWidget Interface

	/// <summary>
	/// Updates the Top Transcription Text, shifting all current transcriptions down.
	/// </summary>
	void UpdateTopTranscription(const FString& InTopTranscription);

protected:

	/// <summary>
	/// The Container of the Transcription Slots.
	/// </summary>
	TWeakPtr<SVerticalBox> TranscriptionContainer;

	/// <summary>
	/// Array of the created Transcription Slots, displaying recieved transcriptions.
	/// </summary>
	TArray<TWeakPtr<STextBlock>> TranscriptionSlots;

};