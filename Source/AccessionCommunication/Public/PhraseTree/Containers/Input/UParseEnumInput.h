// Copyright (C) HCI-BCU 2025. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UParseIntInput.h"

#include "UParseEnumInput.generated.h"

UCLASS()
class ACCESSIONCOMMUNICATION_API UParseEnumInput : public UParseIntInput
{
	GENERATED_BODY()

public:
	UParseEnumInput() = default;
	virtual ~UParseEnumInput()
	{
		delete EnumType;
	};

	/// <summary>
	/// Sets the Enum Type for the Input.
	/// </summary>
	/// <param name="InEnumType">The Enum Type to Set this Input To.</param>
	void SetEnumType(UEnum *InEnumType)
	{
		EnumType = InEnumType;
	}

	/// <summary>
	/// Gets the EnumType Bound To This Input.
	/// </summary>
	/// <param name="OutEnumType">The Bound EnumType To Set.</param>
	void GetEnumType(UEnum *&OutEnumType)
	{
		OutEnumType = EnumType;
	}

	/// <summary>
	/// Gets the EnumType Bound To This Input.
	/// </summary>
	/// <returns>The Bound EnumType of the Input.</returns>
	UEnum *GetEnumType()
	{
		return EnumType;
	}

protected:
	UPROPERTY()
	UEnum *EnumType;
};
