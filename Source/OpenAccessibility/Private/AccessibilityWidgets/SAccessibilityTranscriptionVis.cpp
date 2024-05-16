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

	FSlateFontInfo FontInfo = FAppStyle::GetFontStyle("NormalText");
	FontInfo.Size = 15;

	TSharedPtr<STextBlock> CurrentTranscriptionSlot;
	for (int i = 0; i < TranscriptionSlotAmount; i++)
	{
		TranscriptionHolder->AddSlot()
			.HAlign(HAlign_Center)
			.Padding(4.0f)
			.AutoHeight()
			[
				SAssignNew(CurrentTranscriptionSlot, STextBlock)
					.Text(FText::GetEmpty())
					.Font(FontInfo)
					.SimpleTextMode(true)
					.ColorAndOpacity(i == 0 ? FSlateColor(FLinearColor::Yellow) : FSlateColor(FLinearColor::Gray))
			];

		TranscriptionSlots.Add(CurrentTranscriptionSlot);
	}

	// Construct the Main Component

	SBox::Construct(SBox::FArguments()
		.Padding(FMargin(5.0f))
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
				.ZOrder(1)
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
		]
	);

	this->TranscriptionContainer = TranscriptionHolder;
}

void SAccessibilityTranscriptionVis::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SBox::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

void SAccessibilityTranscriptionVis::UpdateTopTranscription(const FString& InTopTranscription)
{
	FString LastTopText = InTopTranscription;
	FString TempText;

	TSharedPtr<STextBlock> CurrentTranscriptionSlot;
	for (TWeakPtr<STextBlock>& TranscriptionSlot : TranscriptionSlots)
	{
		CurrentTranscriptionSlot = TranscriptionSlot.Pin();

		TempText = FString(CurrentTranscriptionSlot->GetText().ToString());
		CurrentTranscriptionSlot->SetText(FText::FromString(LastTopText));

		CurrentTranscriptionSlot->Invalidate(EInvalidateWidgetReason::PaintAndVolatility);

		LastTopText = TempText;
	}

	TranscriptionContainer.Pin()->Invalidate(EInvalidateWidget::Layout);
}
