// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Input/UParseInput.h"

/// <summary>
/// The Collected Information from the Propogation of the Phrase through the tree.
/// </summary>
struct OPENACCESSIBILITYCOMMUNICATION_API FParseRecord
{
public:
	friend class FPhraseTree;

	FParseRecord()
	{
		PhraseInputs = TMultiMap<FString, UParseInput*>();
		ContextObjectStack = TArray<UObject*>();
	}

	FParseRecord(TArray<UObject*> InContextObjects)
	{
		PhraseInputs = TMultiMap<FString, UParseInput*>();
		ContextObjectStack = InContextObjects;
	}

	~FParseRecord()
	{
		PhraseInputs.Empty();
	}

	UParseInput* GetPhraseInput(const FString& InString)
	{
		return *PhraseInputs.Find(InString);
	}

	template<class CastToType>
	CastToType* GetPhraseInput(const FString& InString)
	{
		return Cast<CastToType>(*PhraseInputs.Find(InString));
	}

	void GetPhraseInput(const FString& InString, UParseInput* OutInput)
	{
		OutInput = *PhraseInputs.Find(InString);
	}

	template<class CastToType>
	void GetPhraseInput(const FString& InString, CastToType* OutInput)
	{
		OutInput = Cast<CastToType>(*PhraseInputs.Find(InString));
	}

	// -- GetPhraseInputs

	void GetPhraseInputs(const FString& InString, TArray<UParseInput*> OutInputs, const bool MaintainOrder = true)
	{
		PhraseInputs.MultiFind(InString, OutInputs, MaintainOrder);
	}

	TArray<UParseInput*> GetPhraseInputs(const FString& InString, const bool MaintainOrder = true)
	{
		TArray<UParseInput*> OutInputs;

		PhraseInputs.MultiFind(InString, OutInputs, MaintainOrder);

		return OutInputs;
	}

	// -- PhraseInput

	void AddPhraseInput(const FString& InString, UParseInput* InInput)
	{
		PhraseInputs.Add(InString, InInput);
	}

	void RemovePhraseInput(const FString& InString)
	{
		PhraseInputs.Remove(InString);
	}

	// -- ContextObject Stack Modification

	void PushContextObj(UObject* InObject)
	{
		this->ContextObjectStack.Push(InObject);
	}

	void PopContextObj()
	{
		this->ContextObjectStack.Pop();
	}

	void PopContextObj(UObject* OutObject)
	{
		OutObject = this->ContextObjectStack.Pop();
	}

	void RemoveContextObj(UObject* InObject)
	{
		this->ContextObjectStack.Remove(InObject);
	}

	// -- HasContextObj
	
	bool HasContextObj()
	{
		return this->ContextObjectStack.Num() > 0;
	}

	bool HasContextObj(UObject* InObject)
	{
		return HasContextObj() && this->ContextObjectStack.Contains(InObject);
	}

	// -- GetContextObj

	UObject* GetContextObj()
	{
		return this->ContextObjectStack.Last();
	}

	void GetContextObj(UObject* OutObject)
	{
		OutObject = this->ContextObjectStack.Last();
	}

	template<class CastToType>
	CastToType* GetContextObj()
	{
		return Cast<CastToType>(this->ContextObjectStack.Last());
	}

	template<class CastToType>
	void GetContextObj(CastToType* OutObject)
	{
		OutObject = Cast<CastToType>(this->ContextObjectStack.Last());
	}

protected:

	TArray<UObject*> ContextObjectStack = TArray<UObject*>();

	TMultiMap<FString, UParseInput*> PhraseInputs;

};