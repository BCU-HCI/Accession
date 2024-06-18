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

	void UpdateIndex(const int32 NewIndex);

	void UpdateIndex(const FString& NewIndex);

	void UpdateIndex(const FText& NewIndex);

	TSharedPtr<STextBlock> GetIndexText() const 
	{
		return IndexTextBlock.IsValid() ? IndexTextBlock.Pin() : TSharedPtr<STextBlock>();
	}

protected:

	TWeakPtr<STextBlock> IndexTextBlock;

};