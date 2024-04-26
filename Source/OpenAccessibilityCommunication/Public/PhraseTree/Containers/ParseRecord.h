// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Input/UParseInput.h"

/*
template<typename InputType>
class OPENACCESSIBILITYCOMMUNICATION_API FParseInput
{

public:

	FParseInput(InputType* InInput)
	{
		Input = MakeUnique<InputType>(InInput);
	}

	FParseInput(InputType& InInput)
	{
		Input = MakeUnique<InputType>(InInput);
	}

	virtual ~FParseInput()
	{
		
	}

	void GetInput(InputType* OutInput)
	{
		OutInput = Input.Get();
	}

	void GetInput(InputType& OutInput)
	{
		OutInput = *Input.Get();
	}

	InputType* GetInput()
	{
		return Input.Get();
	}

private:
	TUniquePtr<InputType> Input;

};
*/

/// <summary>
/// The Collected Information from the Propogation of the Phrase through the tree.
/// </summary>
struct OPENACCESSIBILITYCOMMUNICATION_API FParseRecord
{
public:

	FParseRecord()
	{
		PhraseInputs = TMultiMap<FString, UParseInput*>();
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

	template<class CastToType>
	void GetPhraseInputs(const FString& InString, TArray<CastToType*> OutInputs, const bool MaintainOrder = true)
	{
		PhraseInputs.MultiFind(InString, OutInputs, MaintainOrder);
	}

	TArray<UParseInput*> GetPhraseInputs(const FString& InString, const bool MaintainOrder = true)
	{
		TArray<UParseInput*> OutInputs;

		PhraseInputs.MultiFind(InString, OutInputs, MaintainOrder);

		return OutInputs;
	}

	template<class CastToType>
	TArray<CastToType*> GetPhraseInputs(const FString& InString, const bool MaintainOrder = true)
	{
		TArray<CastToType*> OutInputs;

		PhraseInputs.MultiFind(InString, OutInputs, MaintainOrder);

		return OutInputs;
	}

	// -- AddPhraseInput

	void AddPhraseInput(const FString& InString, UParseInput* InInput)
	{
		PhraseInputs.Add(InString, InInput);
	}

	// -- SetContextObj

	void SetContextObj(UObject* InObject)
	{
		this->ContextObj = InObject;
	}

	// -- GetContextObj

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

protected:

	UObject* ContextObj = nullptr;

	TMultiMap<FString, UParseInput*> PhraseInputs;

};