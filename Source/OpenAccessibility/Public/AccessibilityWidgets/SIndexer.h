// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class OPENACCESSIBILITY_API SIndexer : public SBox {
public:

	SLATE_BEGIN_ARGS( SIndexer )
    : _TextColor(FLinearColor::White)
	, _BorderColor(FLinearColor::Black)
	{}
		SLATE_ARGUMENT(FLinearColor, TextColor)
		SLATE_ARGUMENT(FLinearColor, BorderColor)

		SLATE_PRIVATE_ARGUMENT_VARIABLE(int32, IndexValue) = -1;
		SLATE_PRIVATE_ARGUMENT_FUNCTION(int32, IndexValue)
		SLATE_PRIVATE_ATTRIBUTE_VARIABLE(EVisibility, IndexVisibility) = EVisibility::Visible;
		SLATE_PRIVATE_ATTRIBUTE_FUNCTION(EVisibility, IndexVisibility)
	SLATE_END_ARGS()

	~SIndexer();

	// SWidget Implementation

	virtual void Tick(const FGeometry& AllotedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	
	void Construct(const FArguments& InArgs);

	// End SWidget Implementation

	/// <summary>
	/// Updates the Index Widget with the New Index Value.
	/// </summary>
	/// <param name="NewIndex">- The New Index Value.</param>
	void UpdateIndex(const int32 NewIndex);

	/// <summary>
	/// Updates the Index Widget with the New String Index Value.
	/// </summary>
	/// <param name="NewIndex">- The New Index Value, in String Form.</param>
	void UpdateIndex(const FString& NewIndex);

	/// <summary>
	/// Updates the Index Widget with the New Text Index Value.
	/// </summary>
	/// <param name="NewIndex">- The New Index Value, in Text Form.</param>
	void UpdateIndex(const FText& NewIndex);

	/// <summary>
	/// Gets the Index TextBlock Widget.
	/// </summary>
	/// <returns>A Valid TextBlock Widget, if it is still found. Otherwise InValid SharedPtr.</returns>
	TSharedPtr<STextBlock> GetIndexText() const 
	{
		return IndexTextBlock.IsValid() ? IndexTextBlock.Pin() : TSharedPtr<STextBlock>();
	}

protected:

	/// <summary>
	/// Weak Pointer to the Main TextBlock Widget.
	/// </summary>
	TWeakPtr<STextBlock> IndexTextBlock;

};