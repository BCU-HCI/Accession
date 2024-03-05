// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FPhraseNode;

struct OPENACCESSIBILITY_API FPhraseParseResult
{
	FPhraseParseResult()
	{
		PhraseInputs = TMap<FString, int>();
	}

	~FPhraseParseResult()
	{
		PhraseInputs.Empty();
	}

	TMap<FString, int> PhraseInputs;
};

enum PhrasePropogationType : uint8_t
{
	PHRASE_NOT_PARSED = 0,
	PHRASE_PARSED = 1,
	PHRASE_CANNOT_BE_PARSED = 2,
};

/// <summary>
/// Contained for the Result of Propogating the Phrase through the Tree.
/// </summary>
struct OPENACCESSIBILITY_API FPhrasePropogationResult
{
	FPhrasePropogationResult()
	{
		Result = PHRASE_NOT_PARSED;
		ReachedNode = nullptr;
	}

	FPhrasePropogationResult(PhrasePropogationType InResult)
	{
		Result = InResult;
		ReachedNode = nullptr;
	}

	FPhrasePropogationResult(PhrasePropogationType InResult, TWeakPtr<FPhraseNode> InReachedNode)
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
	TWeakPtr<FPhraseNode> ReachedNode;
};

/**
 * 
 */
class OPENACCESSIBILITY_API FPhraseTree
{
public:
	FPhraseTree();
	~FPhraseTree();

	FPhrasePropogationResult ParsePhrase(const FString InPhrase);

	/// <summary>
	/// Bind a branch to the tree.
	/// Attaching to any overlapping nodes.
	/// </summary>
	/// <param name="InNode">The constructed branch to attach to the tree.</param>
	void BindBranch(const TSharedPtr<FPhraseNode> InNode);
	
	/// <summary>
	/// Bind Multiple Branches to the Tree, that are not connected.
	/// </summary>
	void BindBranches(const TArray<TSharedPtr<FPhraseNode>> InNodes);

private:

	void ConstructPhraseTree();

private:

	/// <summary>
	/// The Root Node of the Current Constructed Tree.
	/// </summary>
	TSharedPtr<FPhraseNode> RootNode;

	/// <summary>
	/// The Node that was last visited in the tree.
	/// Allowing for quick entry, due to split phrases.
	/// </summary>
	TWeakPtr<FPhraseNode> LastVistedNode;

	size_t NodeCount = 0;
};
