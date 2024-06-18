// Copyright F-Dudley. All Rights Reserved.

#include "AccessibilityWidgets/SContentIndexer.h"
#include "AccessibilityWidgets/SIndexer.h"

SContentIndexer::~SContentIndexer()
{
}

void SContentIndexer::Construct(const FArguments& InArgs)
{
	TSharedPtr<SWidget> Content;

	switch (InArgs._IndexPositionToContent)
	{
		case EIndexerPosition::Top:
			Content = ConstructTopIndexer(InArgs);
			break;

		case EIndexerPosition::Bottom:
			Content = ConstructBottomIndexer(InArgs);
			break;

		default:
		case EIndexerPosition::Left:
			Content = ConstructLeftIndexer(InArgs);
			break;

		case EIndexerPosition::Right:
			Content = ConstructRightIndexer(InArgs);
			break;
	}

	ChildSlot
	[
		Content.ToSharedRef()
	];
}

void SContentIndexer::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SBox::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

void SContentIndexer::UpdateIndex(const int32 IndexValue)
{
	if (IndexerWidget.IsValid())
		IndexerWidget.Pin()->UpdateIndex( IndexValue );
}

TSharedPtr<SWidget> SContentIndexer::ConstructTopIndexer(const FArguments& InArgs)
{
	return SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.AutoHeight()
		.Padding(.1f, .25f)
		[
			ConstructIndexContainer(InArgs).ToSharedRef()
		]

		+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.AutoHeight()
		[
			ConstructContentContainer(InArgs._ContentToIndex.ToSharedRef()).ToSharedRef()
		];
}

TSharedPtr<SWidget> SContentIndexer::ConstructBottomIndexer(const FArguments& InArgs)
{
	return SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.AutoHeight()
		[
			ConstructContentContainer(InArgs._ContentToIndex.ToSharedRef()).ToSharedRef()
		]

		+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.AutoHeight()
		.Padding(.1f, .25f)
		[
			ConstructIndexContainer(InArgs).ToSharedRef()
		];
}

TSharedPtr<SWidget> SContentIndexer::ConstructLeftIndexer(const FArguments& InArgs)
{
	return SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		.AutoWidth()
		.Padding(.25f, .1f)
		[
			ConstructIndexContainer(InArgs).ToSharedRef()
		]

		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		.AutoWidth()
		[
			ConstructContentContainer(InArgs._ContentToIndex.ToSharedRef()).ToSharedRef()
		];
}

TSharedPtr<SWidget> SContentIndexer::ConstructRightIndexer(const FArguments& InArgs)
{
	return SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		.AutoWidth()
		[
			ConstructContentContainer(InArgs._ContentToIndex.ToSharedRef()).ToSharedRef()
		]

		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		.AutoWidth()
		.Padding(.25f, .1f)
		[
			ConstructIndexContainer(InArgs).ToSharedRef()
		];
}

TSharedPtr<SWidget> SContentIndexer::ConstructContentContainer(TSharedRef<SWidget> ContentToIndex)
{
	IndexedContent = ContentToIndex;
	return ContentToIndex;
}

TSharedPtr<SWidget> SContentIndexer::ConstructIndexContainer(const FArguments& InArgs, FLinearColor TextColor)
{
	return SAssignNew(IndexerWidget, SIndexer)
	.TextColor(TextColor)
	.BorderColor(FLinearColor::Gray)
	.IndexValue(InArgs._IndexValue)
	.IndexVisibility(InArgs._IndexVisibility);
}

FText SContentIndexer::ConstructIndexText(int32 Index)
{
	return FText::FromString(FString::FromInt(Index));
}
