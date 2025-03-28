// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "UParseInput.generated.h"

UCLASS()
class ACCESSIONCOMMUNICATION_API UParseInput : public UObject
{
	GENERATED_BODY()

public:
	UParseInput() = default;
	virtual ~UParseInput() {

	};
};

// Input Constructor Functions

/// <summary>
/// Creates a new ParseInput Object.
/// </summary>
/// <typeparam name="ParseInputType">The Desired Input Type. (Derrived From UParseInput)</typeparam>
/// <returns>The Constructed ParseInput of the Provided Type.</returns>
template <class ParseInputType>
[[nodiscard]] FORCEINLINE ParseInputType *MakeParseInput()
{
	ParseInputType *NewObj = NewObject<ParseInputType>();
	NewObj->AddToRoot();

	return NewObj;
}