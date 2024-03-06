// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FPhraseNode;

/// <summary>
/// The Collected Information from the Propogation of the Phrase through the tree.
/// </summary>
struct OPENACCESSIBILITY_API FParseRecord
{
	FParseRecord()
	{
		PhraseInputs = TMap<FString, int>();
	}

	~FParseRecord()
	{
		PhraseInputs.Empty();
	}

	TMap<FString, int> PhraseInputs;
};

enum PhrasePropogationType : uint8_t
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
	/// When the Phrase is Parsed all the way to a leaf node.
	/// </summary>
	PHRASE_PARSED = 3,

	/// <summary>
	/// When the Phrase is Parsed all the way to a leaf node, triggering an event.
	/// </summary>
	PHRASE_PARSED_AND_EXECUTED = 4,
};

/// <summary>
/// Contained for the Result of Propogating the Phrase through the Tree.
/// </summary>
struct OPENACCESSIBILITY_API FParseResult
{
	FParseResult()
	{
		Result = PHRASE_NOT_PARSED;
	}

	FParseResult(PhrasePropogationType InResult)
	{
		Result = InResult;
	}

	FParseResult(PhrasePropogationType InResult, TSharedRef<FPhraseNode> InReachedNode)
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
	TSharedRef<FPhraseNode> ReachedNode;
};