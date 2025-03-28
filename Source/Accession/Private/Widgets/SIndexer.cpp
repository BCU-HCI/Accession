// Copyright F-Dudley. All Rights Reserved.

#include "Widgets/SIndexer.h"

SIndexer::~SIndexer()
{
}

void SIndexer::Tick(const FGeometry &AllotedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SBox::Tick(AllotedGeometry, InCurrentTime, InDeltaTime);
}

void SIndexer::Construct(const FArguments &InArgs)
{
	ChildSlot
		[SNew(SBorder)
			 .HAlign(HAlign_Center)
			 .VAlign(VAlign_Center)
			 .Visibility(InArgs._IndexVisibility)
			 .Padding(FMargin(4.f, 2.f))
			 .BorderBackgroundColor(FSlateColor(InArgs._BorderColor))
				 [SAssignNew(IndexTextBlock, STextBlock)
					  .Text(FText::FromString(FString::FromInt(InArgs._IndexValue)))
					  .TextShapingMethod(ETextShapingMethod::KerningOnly)
					  .ColorAndOpacity(FSlateColor(InArgs._TextColor))]];
}

void SIndexer::UpdateIndex(const int32 NewIndex)
{
	if (!IndexTextBlock.IsValid())
		return;

	IndexTextBlock.Pin()->SetText(
		FText::FromString(FString::FromInt(NewIndex)));
}

void SIndexer::UpdateIndex(const FString &NewIndex)
{
	if (!IndexTextBlock.IsValid())
		return;

	IndexTextBlock.Pin()->SetText(
		FText::FromString(NewIndex));
}

void SIndexer::UpdateIndex(const FText &NewIndex)
{
	if (!IndexTextBlock.IsValid())
		return;

	IndexTextBlock.Pin()->SetText(NewIndex);
}
