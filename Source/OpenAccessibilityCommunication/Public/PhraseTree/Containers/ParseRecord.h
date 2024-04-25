// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/// <summary>
/// The Collected Information from the Propogation of the Phrase through the tree.
/// </summary>
struct OPENACCESSIBILITYCOMMUNICATION_API FParseRecord
{
public:

	FParseRecord()
	{
		PhraseInputs = TMultiMap<FString, int>();
	}

	~FParseRecord()
	{
		PhraseInputs.Empty();
	}

	int GetPhraseInput(const FString& InString)
	{
		return *PhraseInputs.Find(InString);
	}

	void GetPhraseInput(const FString& InString, int OutInput)
	{
		OutInput = *PhraseInputs.Find(InString);
	}

	TArray<int> GetPhraseInputs(const FString& InString, const bool MaintainOrder = true)
	{
		TArray<int> OutInputs;

		PhraseInputs.MultiFind(InString, OutInputs, MaintainOrder);

		return OutInputs;
	}

	void GetPhraseInputs(const FString& InString, TArray<int> OutInputs, const bool MaintainOrder = true)
	{
		PhraseInputs.MultiFind(InString, OutInputs, MaintainOrder);
	}

	void SetContextObj(UObject* InObject)
	{
		this->ContextObj = InObject;
	}

	UObject* GetContextObj()
	{
		return this->ContextObj;
	}

	void GetContextObj(UObject* OutObject)
	{
		OutObject = this->ContextObj;
	}

	template<class CastToType>
	CastToType* GetContextObj()
	{
		return Cast<CastToType>(this->ContextObj);
	}

	template<class CastToType>
	void GetContextObj(CastToType* OutObject)
	{
		OutObject = Cast<CastToType>(this->ContextObj);
	}

	// NEEDS TO BE MOVED TO PROTECTED AND ENFORCE FUNCTION USE,
	// NEED TO PROVIDE CLEAN WAY TO ADD INPUTS.
	TMultiMap<FString, int> PhraseInputs;

protected:

	UObject* ContextObj = nullptr;
};