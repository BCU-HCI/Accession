// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FPhraseNode;

typedef TSharedPtr<FPhraseNode> TPhraseNode;

typedef TArray<TPhraseNode> TPhraseNodeArray;


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
/// Contains the Result of Propagation through the Phrase Tree.
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