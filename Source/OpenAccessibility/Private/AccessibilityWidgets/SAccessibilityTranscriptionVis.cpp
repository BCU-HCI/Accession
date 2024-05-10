// Copyright F-Dudley. All Rights Reserved.

#include "AccessibilityWidgets/SAccessibilityTranscriptionVis.h"

SAccessibilityTranscriptionVis::~SAccessibilityTranscriptionVis()
{
}

void SAccessibilityTranscriptionVis::Construct(const FArguments& InArgs)
{
	TSharedPtr<SVerticalBox> TranscriptionHolder = SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.Padding(4.0f)
		.AutoHeight()
		[
			SNew(STextBlock)
				.Text(FText::FromString(TEXT("- - - - -")))
				.ColorAndOpacity(FSlateColor(FLinearColor::Yellow))
		];

	for (int i = 1; i < InArgs._VisAmount; i++)
	{
		TranscriptionHolder->AddSlot()
			.Padding(4.0f)
			.AutoHeight()
			[
				SNew(STextBlock)
					.Text(FText::FromString(TEXT("- - - - -")))
					.ColorAndOpacity(FSlateColor(FLinearColor::Gray)) // Gradient Color
			];
	}

	SBorder::Construct(SBorder::FArguments()
		.BorderImage(FAppStyle::GetBrush("Menu.Background"))
		.Padding(FMargin(5.0f))
		[
			SNew(SBox)
			.MinDesiredWidth(300.0f)
			.MinDesiredHeight(100.0f)
			[
				// Transcription Holder
				SNew(SVerticalBox)

					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(STextBlock)
							.Text(FText::FromString(TEXT("TEST")))
					]

					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(STextBlock)
							.Text(FText::FromString(TEXT("TEST - LINE 2")))
					]
			]
		]
	);
}

void SAccessibilityTranscriptionVis::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{

}
