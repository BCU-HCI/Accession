// Copyright F-Dudley. All Rights Reserved.

#include "Widgets/STranscriptionVis.h"

#include "AccessionCommunication.h"
#include "AudioManager.h"

STranscriptionVis::~STranscriptionVis()
{
}

void STranscriptionVis::Construct(const FArguments &InArgs)
{
	// Transcription Holder
	TSharedPtr<SVerticalBox> TranscriptionHolder = SNew(SVerticalBox) + SVerticalBox::Slot()
																			.Padding(4.0f)
																			.AutoHeight();

	// Verify a least one slot will be constructed
	int TranscriptionSlotAmount = FMath::Max(1, InArgs._VisAmount);

	FSlateFontInfo FontInfo = FAppStyle::GetFontStyle("NormalText");
	FontInfo.Size = 12;

	TSharedPtr<STextBlock> CurrentTranscriptionSlot;
	for (int i = 0; i < TranscriptionSlotAmount; i++)
	{
		TranscriptionHolder->AddSlot()
			.HAlign(HAlign_Center)
			.Padding(4.0f)
			.AutoHeight()
				[SAssignNew(CurrentTranscriptionSlot, STextBlock)
					 .Text(FText::GetEmpty())
					 .Font(FontInfo)
					 .SimpleTextMode(true)
					 .ColorAndOpacity(i == 0 ? FSlateColor(FLinearColor(1.0f, 1.0f, 0, 1.0f)) : FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f)))];

		TranscriptionSlots.Add(CurrentTranscriptionSlot);
	}

	// Construct the Main Component

	UAudioManager *AudioManager = FAccessionCommunicationModule::Get().AudioManager;

	ChildSlot
		.Padding(FMargin(5.0f))
			[SNew(SOverlay) + SOverlay::Slot()
								  .ZOrder(1)
									  [SNew(SBorder)
										   .BorderBackgroundColor(FSlateColor(FLinearColor::Gray))
										   .BorderBackgroundColor_Lambda([AudioManager]()
																		 { return AudioManager != nullptr && AudioManager->IsCapturingAudio()
																					  ? FSlateColor(FLinearColor::Red)
																					  : FSlateColor(FLinearColor::Gray); })
											   [SNew(SBox)
													.MinDesiredWidth(250.0f)
													.MinDesiredHeight(60.0f)
														[TranscriptionHolder.ToSharedRef()]]]];

	this->TranscriptionContainer = TranscriptionHolder;
}

void STranscriptionVis::Tick(const FGeometry &AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SBox::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

void STranscriptionVis::UpdateTopTranscription(const FString &InTopTranscription)
{
	FString LastTopText = InTopTranscription;
	FString TempText;

	TSharedPtr<STextBlock> CurrentTranscriptionSlot;
	for (TWeakPtr<STextBlock> &TranscriptionSlot : TranscriptionSlots)
	{
		CurrentTranscriptionSlot = TranscriptionSlot.Pin();

		TempText = FString(CurrentTranscriptionSlot->GetText().ToString());
		CurrentTranscriptionSlot->SetText(FText::FromString(LastTopText));

		CurrentTranscriptionSlot->Invalidate(EInvalidateWidgetReason::PaintAndVolatility);

		LastTopText = TempText;
	}

	TranscriptionContainer.Pin()->Invalidate(EInvalidateWidget::Layout);
}
