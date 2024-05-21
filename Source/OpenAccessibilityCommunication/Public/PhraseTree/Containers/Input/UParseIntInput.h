// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UParseInput.h"

#include "UParseIntInput.generated.h"

UCLASS()
class OPENACCESSIBILITYCOMMUNICATION_API UParseIntInput : public UParseInput
{
	GENERATED_BODY()

public:

	UParseIntInput() = default;
	virtual ~UParseIntInput()
	{

	};

	/// <summary>
	/// Sets the Value of the Input.
	/// </summary>
	/// <param name="InValue">- The Value to set the Input To.</param>
	void SetValue(int32 InValue)
	{
		Value = InValue;
	}

	/// <summary>
	/// Gets the Current Value of the Input.
	/// </summary>
	/// <param name="OutValue">- Returns the Current Value of the Input.</param>
	void GetValue(int32& OutValue)
	{
		OutValue = Value;
	}

	/// <summary>
	/// Gets the Current Value of the Input.
	/// </summary>
	/// <returns>The Current Value of the Input.</returns>
	int32 GetValue()
	{
		return Value;
	}

protected:

	int32 Value;

};