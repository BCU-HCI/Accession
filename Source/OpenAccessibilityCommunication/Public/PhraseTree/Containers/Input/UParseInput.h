// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UParseInput.generated.h"

UCLASS(Abstract)
class UParseInput : public UObject
{
	GENERATED_BODY()

public:

	UParseInput() = default;
	virtual ~UParseInput()
	{

	};


};

// Input Constructor Functions

template<class ParseInputType>
[[nodiscard]] FORCEINLINE ParseInputType* MakeParseInput()
{
	UParseInput* NewObject = NewObject<ParseInputType>();
	NewObject->AddToRoot();

	return NewObject;
}