// Copyright (C) HCI-BCU. All rights reserved.
// Published under GPLv3 License, 2025. See LICENSE in the Plugin Root for more information.

#pragma once

#include "CoreMinimal.h"
#include "UParseInput.h"

#include "UParseFloatInput.generated.h"

UCLASS()
class ACCESSIONCOMMUNICATION_API UParseFloatInput : public UParseInput
{
	GENERATED_BODY()

public:
	UParseFloatInput() = default;
	virtual ~UParseFloatInput() {

	};

	/// <summary>
	/// Sets the Value of the Input.
	/// </summary>
	/// <param name="InValue">- The Value to set the Input To.</param>
	void SetValue(float InValue)
	{
		Value = InValue;
	}

	/// <summary>
	/// Gets the Current Value of the Input.
	/// </summary>
	/// <param name="OutValue">- Returns the Current Value of the Input.</param>
	void GetValue(float &OutValue)
	{
		OutValue = Value;
	}

	/// <summary>
	/// Gets the Current Value of the Input.
	/// </summary>
	/// <returns>The Current Value of the Input.</returns>
	float GetValue()
	{
		return Value;
	}

protected:
	float Value;
};