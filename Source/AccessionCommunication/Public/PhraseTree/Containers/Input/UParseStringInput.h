// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UParseInput.h"

#include "UParseStringInput.generated.h"

UCLASS()
class ACCESSIONCOMMUNICATION_API UParseStringInput : public UParseInput
{
	GENERATED_BODY()

public:
	UParseStringInput() = default;
	virtual ~UParseStringInput() {

	};

	/// <summary>
	/// Sets the Value of the Input.
	/// </summary>
	/// <param name="InValue">- The Value to set the Input To.</param>
	void SetValue(FString InValue)
	{
		Value = InValue;
	}

	/// <summary>
	/// Gets the Value of the Input.
	/// </summary>
	/// <param name="OutValue">- Returns the Current Value of the Input.</param>
	void GetValue(FString &OutValue)
	{
		OutValue = Value;
	}

	/// <summary>
	/// Gets the Value of the Input.
	/// </summary>
	/// <returns></returns>
	FString GetValue()
	{
		return Value;
	}

protected:
	FString Value;
};