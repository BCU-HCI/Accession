// Copyright F-Dudley. All Rights Reserved.

#include "AccessibilityWidgets/SAccessibilityTranscriptionVis.h"

SAccessibilityTranscriptionVis::~SAccessibilityTranscriptionVis()
{
}

void SAccessibilityTranscriptionVis::Construct(const FArguments& InArgs)
{
	SBorder::Construct(SBorder::FArguments()
		.BorderImage(FAppStyle::GetBrush("Menu.Background"))
		.Padding(FMargin(5.0f))
		[
			SNew(SBox)
			.MinDesiredWidth(250.0f)
			.MinDesiredHeight(100.0f)
			[
				// Transcription Holder
				SNew(SVerticalBox)

					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(STextBlock)
							.Text(FText::GetEmpty())
					]

					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(STextBlock)
							.Text(FText::GetEmpty())
					]
			]
		]
	);
}

void SAccessibilityTranscriptionVis::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{

}
