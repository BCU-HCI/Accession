// Copyright F-Dudley. All Rights Reserved.

#include "PhraseTree/Utils.h"
#include "AccessionComLogging.h"

bool NumericParser::IsValidNumeric(const FString &StringToCheck, bool ConvertToUpper)
{
	return StringMappings.Contains(ConvertToUpper ? StringToCheck.ToUpper() : StringToCheck);
}

void NumericParser::StringToNumeric(FString &NumericString, bool ConvertToUpper)
{
	if (const FString *FoundMapping = StringMappings.Find(NumericString))
	{
		NumericString = ConvertToUpper ? *FoundMapping->ToUpper() : *FoundMapping;
	}
	else
		UE_LOG(LogAccessionCom, Warning, TEXT("|| Numeric Parser || No Mapping Found for String: %s ||"), *NumericString);
}

const TMap<const FString, const FString> NumericParser::StringMappings = TMap<const FString, const FString>{
	{TEXT("ZERO"), TEXT("0")},
	{TEXT("ONE"), TEXT("1")},
	{TEXT("TWO"), TEXT("2")},
	{TEXT("TOO"), TEXT("2")},
	{TEXT("TO"), TEXT("2")},
	{TEXT("THREE"), TEXT("3")},
	{TEXT("FOUR"), TEXT("4")},
	{TEXT("FOR"), TEXT("4")},
	{TEXT("FIVE"), TEXT("5")},
	{TEXT("SIX"), TEXT("6")},
	{TEXT("SEVEN"), TEXT("7")},
	{TEXT("EIGHT"), TEXT("8")},
	{TEXT("NINE"), TEXT("9")},
	{TEXT("TEN"), TEXT("10")},
	{TEXT("TIN"), TEXT("10")},
	{TEXT("ELEVEN"), TEXT("11")},
	{TEXT("TWELVE"), TEXT("12")},
	{TEXT("THIRTEEN"), TEXT("13")},
	{TEXT("FOURTEEN"), TEXT("14")},
	{TEXT("FIFTEEN"), TEXT("15")},
	{TEXT("SIXTEEN"), TEXT("16")},
	{TEXT("SEVENTEEN"), TEXT("17")},
	{TEXT("EIGHTEEN"), TEXT("18")},
	{TEXT("NINETEEN"), TEXT("19")},
	{TEXT("TWENTY"), TEXT("20")},
	{TEXT("THIRTY"), TEXT("30")},
	{TEXT("FORTY"), TEXT("40")},
	{TEXT("FIFTY"), TEXT("50")},
	{TEXT("SIXTY"), TEXT("60")},
	{TEXT("SEVENTY"), TEXT("70")},
	{TEXT("EIGHTY"), TEXT("80")},
	{TEXT("NINETY"), TEXT("90")},
	{TEXT("HUNDRED"), TEXT("100")},
};