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
		: _IndexValue(0)
		, _IndexPositionToContent(IndexerPosition::Left)
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

	/// <summary>
	/// Updates the Index Value Displayed on the Indexer Text Widget.
	/// </summary>
	/// <param name="IndexValue">The New Interger Index to Show.</param>
	void UpdateIndex(const int32 IndexValue);

protected:

	/// <summary>
	/// Constructs the Indexer Widget with the Index on Top of the Content.
	/// </summary>
	/// <param name="IndexValue">The Index Value to Index.</param>
	/// <param name="ContentToIndex">The Content that the Indexer is Wrapping.</param>
	/// <returns></returns>
	TSharedPtr<SWidget> ConstructTopIndexer(int32 IndexValue, TSharedRef<SWidget> ContentToIndex);

	/// <summary>
	/// Constructs the Indexer Widget with the Index Below the Content.
	/// </summary>
	/// <param name="IndexValue">The Index Value to Index.</param>
	/// <param name="ContentToIndex">The Content that the Indexer is Wrapping.</param>
	/// <returns></returns>
	TSharedPtr<SWidget> ConstructBottomIndexer(int32 IndexValue, TSharedRef<SWidget> ContentToIndex);

	/// <summary>
	/// Constructs the Indexer Widget with the Index to the Left of the Content.
	/// </summary>
	/// <param name="IndexValue">The Index Value to Index.</param>
	/// <param name="ContentToIndex">The Content that the Indexer is Wrapping.</param>
	/// <returns></returns>
	TSharedPtr<SWidget> ConstructLeftIndexer(int32 IndexValue, TSharedRef<SWidget> ContentToIndex);

	/// <summary>
	/// Constructs the Indexer Widget with the Index to the Right of the Content.
	/// </summary>
	/// <param name="IndexValue">The Index Value to Index.</param>
	/// <param name="ContentToIndex">The Content that the Indexer is Wrapping.</param>
	/// <returns></returns>
	TSharedPtr<SWidget> ConstructRightIndexer(int32 IndexValue, TSharedRef<SWidget> ContentToIndex);

	/// <summary>
	/// Constructs the Container for the Indexer witht the provided Content.
	/// </summary>
	/// <param name="ContentToIndex">The Content that needs to be wrapped with an Indexer Widget.</param>
	/// <returns></returns>
	TSharedPtr<SWidget> ConstructContentContainer(TSharedRef<SWidget> ContentToIndex);

	/// <summary>
	/// Constructs the Indexer Widget with the provided Index Value.
	/// </summary>
	/// <param name="IndexValue">The Index Value to be displayed in the Indexer Widget.</param>
	/// <param name="TextColor">The Color of the Text displaying the Index.</param>
	/// <returns></returns>
	TSharedPtr<SWidget> ConstructIndexContainer(int32 IndexValue, FLinearColor TextColor = FLinearColor::White);

	/// <summary>
	/// Creates the Text Element of the Provided Index Value.
	/// </summary>
	/// <param name="Index">The Index to convert into Text.</param>
	/// <returns></returns>
	FText ConstructIndexText(int32 Index);

protected:

	/// <summary>
	/// The Text Block that Displays the Index Value.
	/// </summary>
	TWeakPtr<STextBlock> IndexTextBlock;
};