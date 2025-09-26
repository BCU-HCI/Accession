// Copyright (C) HCI-BCU. All rights reserved.
// Published under GPLv3 License, 2025. See LICENSE in the Plugin Root for more information.

#pragma once

#include "CoreMinimal.h"
#include "Styling/AppStyle.h"
#include "Widgets/Layout/SBorder.h"

class ACCESSION_API STranscriptionVis : public SBox
{
public:
	SLATE_BEGIN_ARGS(STranscriptionVis)
		: _VisAmount(1)
	{
	}
	SLATE_ARGUMENT(int, VisAmount)
	SLATE_END_ARGS()

	~STranscriptionVis();

	void Construct(const FArguments &InArgs);

	// SWidget Interface

	virtual void Tick(const FGeometry &AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	// End of SWidget Interface

	/// <summary>
	/// Updates the Top Transcription Text, shifting all current transcriptions down.
	/// </summary>
	void UpdateTopTranscription(const FString &InTopTranscription);

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