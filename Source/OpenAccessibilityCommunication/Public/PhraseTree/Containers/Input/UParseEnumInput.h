// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UParseIntInput.h"

#include "UParseEnumInput.generated.h"

UCLASS()
class UParseEnumInput : public UParseIntInput
{
	GENERATED_BODY()

public:

	UParseEnumInput() = default;
	virtual ~UParseEnumInput()
	{
		delete EnumType;
	};

	void SetEnumType(UEnum* InEnumType)
	{
		EnumType = InEnumType;
	}

	void GetEnumType(UEnum*& OutEnumType)
	{
		OutEnumType = EnumType;
	}

	UEnum* GetEnumType()
	{
		return EnumType;
	}

protected:
	UEnum* EnumType;

};
