// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

enum class EIndexerPosition : uint8
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
		, _IndexPositionToContent(EIndexerPosition::Left)
		, _ContentToIndex(SNullWidget::NullWidget)
		{}
		SLATE_ARGUMENT(int32, IndexValue)
		SLATE_ARGUMENT(EIndexerPosition, IndexPositionToContent)
		SLATE_ARGUMENT(TSharedPtr<SWidget>, ContentToIndex)

		SLATE_PRIVATE_ATTRIBUTE_VARIABLE(EVisibility, IndexVisibility) = EVisibility::Visible;
		SLATE_PRIVATE_ATTRIBUTE_FUNCTION(EVisibility, IndexVisibility)
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

	/// <summary>
	/// Gets the Current Content Being Indexed.
	/// </summary>
	/// <returns>A Shared Ptr of the Indexed Content</returns>
	TSharedRef<SWidget> GetContent() const 
	{
		return IndexedContent.Pin().ToSharedRef();
    }

	/// <summary>
    /// Gets the Current Content Being Indexed and Casts it to the Provided
    /// Type.
	/// </summary>
	/// <typeparam name="CastType">The Type To Cast The Stored Value To.</typeparam>
	/// <returns>The Casted SharedReference.</returns>
	template<typename CastType>
	TSharedRef<CastType> GetContent() const 
	{
		return CastStaticSharedPtr<CastType>(IndexedContent.Pin());
	}

protected:

	/// <summary>
	/// Constructs the Indexer Widget with the Index on Top of the Content.
	/// </summary>
	/// <param name="IndexValue">The Index Value to Index.</param>
	/// <param name="ContentToIndex">The Content that the Indexer is Wrapping.</param>
	/// <returns></returns>
	TSharedPtr<SWidget> ConstructTopIndexer(const FArguments& InArgs);

	/// <summary>
	/// Constructs the Indexer Widget with the Index Below the Content.
	/// </summary>
	/// <param name="IndexValue">The Index Value to Index.</param>
	/// <param name="ContentToIndex">The Content that the Indexer is Wrapping.</param>
	/// <returns></returns>
	TSharedPtr<SWidget> ConstructBottomIndexer(const FArguments& InArgs);

	/// <summary>
	/// Constructs the Indexer Widget with the Index to the Left of the Content.
	/// </summary>
	/// <param name="IndexValue">The Index Value to Index.</param>
	/// <param name="ContentToIndex">The Content that the Indexer is Wrapping.</param>
	/// <returns></returns>
	TSharedPtr<SWidget> ConstructLeftIndexer(const FArguments& InArgs);

	/// <summary>
	/// Constructs the Indexer Widget with the Index to the Right of the Content.
	/// </summary>
	/// <param name="IndexValue">The Index Value to Index.</param>
	/// <param name="ContentToIndex">The Content that the Indexer is Wrapping.</param>
	/// <returns></returns>
	TSharedPtr<SWidget> ConstructRightIndexer(const FArguments& InArgs);

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
	TSharedPtr<SWidget> ConstructIndexContainer(const FArguments& InArgs, FLinearColor TextColor = FLinearColor::White);

	/// <summary>
	/// Creates the Text Element of the Provided Index Value.
	/// </summary>
	/// <param name="Index">The Index to convert into Text.</param>
	/// <returns></returns>
	FText ConstructIndexText(int32 Index);

protected:

	/// <summary>
	/// The Content That The Indexer Is Currently Indexing.
	/// </summary>
	TWeakPtr<SWidget> IndexedContent;

	/// <summary>
	/// The Text Block that Displays the Index Value.
	/// </summary>
	TWeakPtr<STextBlock> IndexTextBlock;
};