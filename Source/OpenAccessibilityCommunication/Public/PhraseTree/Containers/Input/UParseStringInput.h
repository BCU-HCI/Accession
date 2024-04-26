// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UParseInput.h"

#include "UParseStringInput.generated.h"

UCLASS()
class UParseStringInput : public UParseInput
{
	GENERATED_BODY()

public:

	UParseStringInput() = default;
	virtual ~UParseStringInput()
	{

	};

	void SetValue(FString InValue)
	{
		Value = InValue;
	}

	void GetValue(FString& OutValue)
	{
		OutValue = Value;
	}

	FString GetValue()
	{
		return Value;
	}

protected:

	FString Value;
};