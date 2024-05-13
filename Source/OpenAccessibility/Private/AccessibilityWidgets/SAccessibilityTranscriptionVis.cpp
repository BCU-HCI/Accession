// Copyright F-Dudley. All Rights Reserved.

#include "AccessibilityWidgets/SAccessibilityTranscriptionVis.h"

SAccessibilityTranscriptionVis::~SAccessibilityTranscriptionVis()
{
}

void SAccessibilityTranscriptionVis::Construct(const FArguments& InArgs)
{
	// Transcription Holder
	TSharedPtr<SVerticalBox> TranscriptionHolder = SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.Padding(4.0f)
		.AutoHeight();

	// Verify a least one slot will be constructed
	int TranscriptionSlotAmount = 1;
	TranscriptionSlotAmount = FMath::Max(1, InArgs._VisAmount);

	for (int i = 0; i < TranscriptionSlotAmount; i++)
	{
		TSharedPtr<STextBlock> TranscriptionSlot = SNew(STextBlock)
			.Text(FText::FromString(TEXT("- - - - -")))
			.ColorAndOpacity(i == 0 ? FSlateColor(FLinearColor::Yellow) : FSlateColor(FLinearColor::Gray));

		TranscriptionHolder->AddSlot()
			.HAlign(HAlign_Center)
			.Padding(4.0f)
			.AutoHeight()
			[
				TranscriptionSlot.ToSharedRef()
			];

		TranscriptionSlots.Add(TranscriptionSlot.Get());
	}

	// Construct the Main Component

	SOverlay::Construct(SOverlay::FArguments()
		+ SOverlay::Slot()
		.ZOrder(10)
		.Padding(FMargin(5.0f))
		[
			SNew(SBorder)
				.BorderBackgroundColor(FSlateColor(FLinearColor::Gray))
				[
					SNew(SBox)
						.MinDesiredWidth(250.0f)
						.MinDesiredHeight(60.0f)
						[
							TranscriptionHolder.ToSharedRef()
						]
				]
			
		]
	);

	this->TranscriptionContainer = TranscriptionHolder;
}

void SAccessibilityTranscriptionVis::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SOverlay::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

void SAccessibilityTranscriptionVis::UpdateTopTranscription(const FString& InTopTranscription)
{
	FText LastTopText = FText::FromString(InTopTranscription);
	FText TempText = FText::GetEmpty();

	for (STextBlock*& TranscriptionSlot : TranscriptionSlots)
	{
		TempText = TranscriptionSlot->GetText();
		TranscriptionSlot->SetText(LastTopText);

		LastTopText = TempText;
	}

}
