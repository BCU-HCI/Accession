// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class IndexerPosition : uint8
{
	Top,
	Bottom,
	Left,
	Right
};

class OPENACCESSIBILITY_API SContentIndexer : public SBox
{
public:

	SLATE_BEGIN_ARGS( SContentIndexer )
		{}
		SLATE_ARGUMENT(int32, IndexValue)
		SLATE_ARGUMENT(IndexerPosition, IndexPositionToContent)
		SLATE_ARGUMENT(TSharedPtr<SWidget>, ContentToIndex)
	SLATE_END_ARGS()

	~SContentIndexer();


	void Construct(const FArguments& InArgs);

	// SWidget Implementation

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	// End SWidget Implementation

	void UpdateIndex(const int32 IndexValue);

protected:

	TSharedPtr<SWidget> ConstructTopIndexer(int32 IndexValue, TSharedRef<SWidget> ContentToIndex);

	TSharedPtr<SWidget> ConstructBottomIndexer(int32 IndexValue, TSharedRef<SWidget> ContentToIndex);

	TSharedPtr<SWidget> ConstructLeftIndexer(int32 IndexValue, TSharedRef<SWidget> ContentToIndex);

	TSharedPtr<SWidget> ConstructRightIndexer(int32 IndexValue, TSharedRef<SWidget> ContentToIndex);

	TSharedPtr<SWidget> ConstructContentContainer(TSharedRef<SWidget> ContentToIndex);

	TSharedPtr<SWidget> ConstructIndexContainer(int32 IndexValue, FLinearColor TextColor = FLinearColor::White);

	FText ConstructIndexText(int32 Index);

protected:

	TWeakPtr<STextBlock> IndexTextBlock;
};