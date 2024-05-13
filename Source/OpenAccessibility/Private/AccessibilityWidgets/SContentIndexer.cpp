// Copyright F-Dudley. All Rights Reserved.

#include "AccessibilityWidgets/SContentIndexer.h"

SContentIndexer::~SContentIndexer()
{
}

void SContentIndexer::Construct(const FArguments& InArgs)
{
	TSharedPtr<SWidget> Content;

	switch (InArgs._IndexPositionToContent)
	{
		case IndexerPosition::Top:
			Content = ConstructTopIndexer(InArgs._IndexValue, InArgs._ContentToIndex.ToSharedRef());
			break;

		case IndexerPosition::Bottom:
			Content = ConstructBottomIndexer(InArgs._IndexValue, InArgs._ContentToIndex.ToSharedRef());
			break;

		default:
		case IndexerPosition::Left:
			Content = ConstructLeftIndexer(InArgs._IndexValue, InArgs._ContentToIndex.ToSharedRef());
			break;

		case IndexerPosition::Right:
			Content = ConstructRightIndexer(InArgs._IndexValue, InArgs._ContentToIndex.ToSharedRef());
			break;
	}

	SBox::Construct(
		SBox::FArguments()
		[
			Content.ToSharedRef()
		]
	); 
}

void SContentIndexer::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SBox::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

void SContentIndexer::UpdateIndex(const int32 IndexValue)
{
	if (IndexTextBlock.IsValid())
		IndexTextBlock.Pin()->SetText( ConstructIndexText(IndexValue) );
}

TSharedPtr<SWidget> SContentIndexer::ConstructTopIndexer(int32 IndexValue, TSharedRef<SWidget> ContentToIndex)
{
	return SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			ConstructIndexContainer(IndexValue).ToSharedRef()
		]

		+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			ConstructContentContainer(ContentToIndex).ToSharedRef()
		];
}

TSharedPtr<SWidget> SContentIndexer::ConstructBottomIndexer(int32 IndexValue, TSharedRef<SWidget> ContentToIndex)
{
	return SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.AutoHeight()
		[
			ConstructContentContainer(ContentToIndex).ToSharedRef()
		]

		+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.AutoHeight()
		[
			ConstructIndexContainer(IndexValue).ToSharedRef()
		];
}

TSharedPtr<SWidget> SContentIndexer::ConstructLeftIndexer(int32 IndexValue, TSharedRef<SWidget> ContentToIndex)
{
	return SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		.AutoWidth()
		[
			ConstructIndexContainer(IndexValue).ToSharedRef()
		]

		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		.AutoWidth()
		[
			ConstructContentContainer(ContentToIndex).ToSharedRef()
		];
}

TSharedPtr<SWidget> SContentIndexer::ConstructRightIndexer(int32 IndexValue, TSharedRef<SWidget> ContentToIndex)
{
	return SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		.AutoWidth()
		[
			ConstructContentContainer(ContentToIndex).ToSharedRef()
		]

		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		.AutoWidth()
		[
			ConstructIndexContainer(IndexValue).ToSharedRef()
		];
}

TSharedPtr<SWidget> SContentIndexer::ConstructContentContainer(TSharedRef<SWidget> ContentToIndex)
{
	return ContentToIndex;
}

TSharedPtr<SWidget> SContentIndexer::ConstructIndexContainer(int32 IndexValue, FLinearColor TextColor)
{
	return SAssignNew(IndexTextBlock, STextBlock)
		.Text( ConstructIndexText(IndexValue) )
		.ColorAndOpacity(FSlateColor(TextColor));
}

FText SContentIndexer::ConstructIndexText(int32 Index)
{
	return FText::FromString("[" + FString::FromInt(Index) + " ]");
}
