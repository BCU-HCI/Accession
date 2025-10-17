// Copyright (C) HCI-BCU 2025. All rights reserved.

#pragma once

#include "CoreMinimal.h"

class ACCESSIONCOMMUNICATION_API NumericParser
{
public:
	/// <summary>
	/// Checks if the String is a Valid Numeric in Comparison to its String Permutations.
	/// </summary>
	/// <param name="StringToCheck">- The String To Check if it is a Numeric.</param>
	/// <param name="ConvertToUpper">- Should The String Be Converted To Upper before Comparison.</param>
	/// <returns></returns>
	static bool IsValidNumeric(const FString &StringToCheck, bool ConvertToUpper = true);

	/// <summary>
	/// Converts a String to its Numeric Permutation.
	/// </summary>
	/// <param name="NumericString">- The String To Convert To Numeric.</param>
	/// <param name="ConvertToUpper">- Should The String Be Converted To Upper before Conversion.</param>
	static void StringToNumeric(FString &NumericString, bool ConvertToUpper = true);

private:
	static const TMap<const FString, const FString> StringMappings;
};
