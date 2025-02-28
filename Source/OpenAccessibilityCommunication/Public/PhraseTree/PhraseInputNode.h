// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhraseTree/PhraseNode.h"
#include "PhraseTree/Utils.h"
#include "OpenAccessibilityComLogging.h"

#include "PhraseTree/Containers/Input/UParseIntInput.h"

/**
 * 
 */
template <typename InputType = int32>
class OPENACCESSIBILITYCOMMUNICATION_API FPhraseInputNode : public FPhraseNode
{
public:
	FPhraseInputNode(const TCHAR* InInputString)
		: FPhraseNode(InInputString)
	{
		
	}

	FPhraseInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes)
		: FPhraseNode(InInputString, InChildNodes)
	{
		
	}

	FPhraseInputNode(const TCHAR* InInputString, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes)
		: FPhraseNode(InInputString, InOnPhraseParsed, InChildNodes)
	{

	}

	FPhraseInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes, TDelegate<void (InputType Input)> InOnInputRecieved)
		: FPhraseNode(InInputString, InChildNodes)
	{
		OnInputReceived = InOnInputRecieved;
	}

	FPhraseInputNode(const TCHAR* InInputString, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes, TDelegate<void(InputType Input)> InOnInputRecieved)
		: FPhraseNode(InInputString, InOnPhraseParsed, InChildNodes)
	{
		OnInputReceived = InOnInputRecieved;
	}

	virtual ~FPhraseInputNode() override
	{
		
	}

	// FPhraseNode Implementation
	
	virtual bool RequiresPhrase(const FString InPhrase) override
	{
		return MeetsInputRequirements(InPhrase);
	}

	virtual bool RequiresPhrase(const FString InPhrase, int32& OutDistance) override
	{
		bool bMeetsRequirements = MeetsInputRequirements(InPhrase);
		OutDistance = bMeetsRequirements ? 0 : INT32_MAX;

		return bMeetsRequirements;
	}

	virtual FParseResult ParsePhrase(TArray<FString>& InPhraseArray, FParseRecord& InParseRecord) override
	{
		if (InPhraseArray.Num() == 0)
		{
			UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Emptied Phrase Array ||"))

				return FParseResult(PHRASE_REQUIRES_MORE, AsShared());
		}

		if (MeetsInputRequirements(InPhraseArray.Last()))
		{
			// Get the Input String.
			FString InputToRecord = InPhraseArray.Pop();

			// Append the Input String to the Record.
			InParseRecord.AddPhraseString(InputToRecord);

			if (!InputToRecord.IsNumeric() && NumericParser::IsValidNumeric(InputToRecord, false))
			{
				NumericParser::StringToNumeric(InputToRecord, false);
			}

			if (!RecordInput(InputToRecord, InParseRecord))
			{
				UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Unable to Record Input ||"))

					return FParseResult(PHRASE_UNABLE_TO_PARSE, AsShared());
			}

			OnPhraseParsed.ExecuteIfBound(InParseRecord);

			return ParseChildren(InPhraseArray, InParseRecord);
		}

		return FParseResult(PHRASE_UNABLE_TO_PARSE, AsShared());
	}

	// End FPhraseNode Implementation

	TDelegate<void(InputType ReceivedInput)> OnInputReceived;

protected:

	/// <summary>
	/// Checks if the Given Phrase Meets Requirements for usage as Input.
	/// In Correlation to this Nodes Input Specifications.
	/// </summary>
	/// <param name="InPhrase">- The Phrase To Check If It Meets Requirements.</param>
	/// <returns>True, if the Phrase Meets Requirements. Otherwise False.</returns>
	virtual bool MeetsInputRequirements(const FString& InPhrase)
	{
		return InPhrase.IsNumeric() || NumericParser::IsValidNumeric(InPhrase, false);
	}

	/// <summary>
	/// Records the Input onto the Parse Record.
	/// </summary>
	/// <param name="InInput">- The Phrase To Record onto the Parse Record.</param>
	/// <param name="OutParseRecord">- Returns the Updated ParseRecord.</param>
	/// <returns>True, if the Input Was Successful in Recording. Otherwise False.</returns>
	virtual bool RecordInput(const FString& InInput, FParseRecord& OutParseRecord)
	{
		return false;
	}

	template<int32> bool RecordInput(const FString& InInput, FParseRecord& OutParseRecord)
	{
		int32 Input = FCString::Atoi(*InInput);

		UParseIntInput* ParseInput = MakeParseInput<UParseIntInput>();
		ParseInput->SetValue(Input);

		OutParseRecord.AddPhraseInput(BoundPhrase, ParseInput);

		OnInputReceived.ExecuteIfBound(Input);

		return true;
	}
};
