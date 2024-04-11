// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class OPENACCESSIBILITYCOMMUNICATION_API NumericParser
{
public:
	static bool IsValidNumeric(const FString& StringToCheck, bool ConvertToUpper = true);

	static void StringToNumeric(FString& NumericString, bool ConvertToUpper = true);

private:
	static const TMap<const FString, const TCHAR*> StringMappings;
};
