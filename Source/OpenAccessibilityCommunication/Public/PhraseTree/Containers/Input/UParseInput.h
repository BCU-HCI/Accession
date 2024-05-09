// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "UParseInput.generated.h"

UCLASS()
class OPENACCESSIBILITYCOMMUNICATION_API UParseInput : public UObject
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
	ParseInputType* NewObj = NewObject<ParseInputType>();
	NewObj->AddToRoot();

	return NewObj;
}