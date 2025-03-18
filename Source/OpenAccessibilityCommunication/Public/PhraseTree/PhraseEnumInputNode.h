// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhraseInputNode.h"

#include "PhraseTree/Containers/Input/UParseEnumInput.h"

/**
 * 
 */
template<typename TEnum>
class OPENACCESSIBILITYCOMMUNICATION_API FPhraseEnumInputNode : public FPhraseInputNode<int32>
{
public:
	static_assert(TIsEnum<TEnum>::Value, "Passed EnumType Must be an Enum.");

	FPhraseEnumInputNode(const TCHAR* InInputString)
		: FPhraseInputNode(InInputString)
	{

	}

	FPhraseEnumInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes)
		: FPhraseInputNode(InInputString, InChildNodes)
	{

	}

	FPhraseEnumInputNode(const TCHAR* InInputString, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes)
		: FPhraseInputNode(InInputString, InOnPhraseParsed, InChildNodes)
	{
		
	}

	FPhraseEnumInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes, TDelegate<void(int32 Input)> InOnInputRecieved)
		: FPhraseInputNode(InInputString, InChildNodes, InOnInputRecieved)
	{
		
	}

	FPhraseEnumInputNode(const TCHAR* InInputString, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes, TDelegate<void(int32 Input)> InOnInputRecieved)
		: FPhraseInputNode(InInputString, InOnPhraseParsed, InChildNodes, InOnInputRecieved)
	{

	}

	virtual ~FPhraseEnumInputNode() override
	{
		
	}

protected:

	/// <summary>
	/// Checks if the Given Phrase Meets Requirements for usage as Input.
	/// In Correlation to this Nodes Input Specifications.
	/// </summary>
	/// <param name="InPhrase">- The Phrase To Check If It Meets Requirements.</param>
	/// <returns>True, if the Phrase Meets Requirements. Otherwise False.</returns>
	virtual bool MeetsInputRequirements(const FString& InPhrase) override
	{
		UEnum* EnumPtr = StaticEnum<TEnum>();
		if (!EnumPtr)
		{
			UE_LOG(LogTemp, Error, TEXT("FPhraseEnumInputNode::MeetsInputRequirements: EnumPtr is NULL"));
			return false;
		}

		return EnumPtr->IsValidEnumName(*EnumPtr->GenerateFullEnumName(*InPhrase.ToUpper()));
	}
	
	/// <summary>
	/// Records the Input onto the Parse Record.
	/// </summary>
	/// <param name="InInput">- The Phrase To Record onto the Parse Record.</param>
	/// <param name="OutParseRecord">- Returns the Updated ParseRecord.</param>
	/// <returns>True, if the Input Was Successful in Recording. Otherwise False.</returns>
	virtual bool RecordInput(const FString& InInput, FParseRecord& OutParseRecord) override
	{
		UEnum* EnumPtr = StaticEnum<TEnum>();
		if (!EnumPtr)
		{
			UE_LOG(LogTemp, Error, TEXT("FPhraseEnumInputNode::RecordInput: EnumPtr is NULL"));
			return false;
		}

		int32 Val = EnumPtr->GetValueByNameString(EnumPtr->GenerateFullEnumName(*InInput.ToUpper()));
		if (Val == INDEX_NONE)
		{
			return false;
		}

		UParseEnumInput* ParseInput = MakeParseInput<UParseEnumInput>();
		ParseInput->SetValue(Val);
		ParseInput->SetEnumType(EnumPtr);

		OutParseRecord.AddPhraseInput(BoundPhrase, ParseInput);

		return true;
	}
};