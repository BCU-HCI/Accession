// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FPhraseNode;

typedef TSharedPtr<FPhraseNode> TPhraseNode;

typedef TArray<TPhraseNode> TPhraseNodeArray;

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

enum OPENACCESSIBILITYCOMMUNICATION_API PhrasePropogationType : uint8_t
{
	/// <summary>
	/// When the given phrase was not attempted to be parsed.
	/// </summary>
	PHRASE_NOT_PARSED = 0,

	/// <summary>
	/// When the given phrase is unable to be parsed.
	/// </summary>
	PHRASE_UNABLE_TO_PARSE = 1,

	/// <summary>
	/// When more phrase components are required to propogate down the tree further.
	/// </summary>
	PHRASE_REQUIRES_MORE = 2,

	/// <summary>
	/// When a phrase requires more correct phrase components to propogate down the tree further, 
	/// as the propogation ran wrong inputs.
	/// </summary>
	PHRASE_REQUIRES_MORE_CORRECT_PHRASES = 3,

	/// <summary>
	/// When the Phrase is Parsed all the way to a leaf node.
	/// </summary>
	PHRASE_PARSED = 4,

	/// <summary>
	/// When the Phrase is Parsed all the way to a leaf node, triggering an event.
	/// </summary>
	PHRASE_PARSED_AND_EXECUTED = 5,
};

/// <summary>
/// Contains the Result of Propogating the Phrase through the Tree.
/// </summary>
struct OPENACCESSIBILITYCOMMUNICATION_API FParseResult
{
	FParseResult()
	{
		Result = PHRASE_NOT_PARSED;
	}

	FParseResult(PhrasePropogationType InResult)
	{
		Result = InResult;
	}

	FParseResult(PhrasePropogationType InResult, TSharedPtr<FPhraseNode> InReachedNode)
	{
		Result = InResult;
		ReachedNode = InReachedNode;
	}

public:

	/// <summary>
	/// The Result of the Propogation.
	/// </summary>
	uint8_t Result;

	/// <summary>
	/// The Node that was reached in the tree.
	/// </summary>
	TSharedPtr<FPhraseNode> ReachedNode;
};