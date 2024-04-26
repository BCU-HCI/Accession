// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UParseInput.h"

#include "UParseIntInput.generated.h"

UCLASS()
class UParseIntInput : public UParseInput
{
	GENERATED_BODY()

public:

	UParseIntInput() = default;
	virtual ~UParseIntInput()
	{

	};

	void SetValue(int32 InValue)
	{
		Value = InValue;
	}

	void GetValue(int32& OutValue)
	{
		OutValue = Value;
	}

	int32 GetValue()
	{
		return Value;
	}

protected:

	int32 Value;

};