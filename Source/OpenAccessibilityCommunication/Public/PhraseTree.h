// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhraseTree/Containers.h"

class FPhraseNode;

/**
 * 
 */
class OPENACCESSIBILITYCOMMUNICATION_API FPhraseTree
{
public:
	FPhraseTree();
	~FPhraseTree();

	FParseResult ParsePhrase(const FString InPhrase);

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
	TSharedPtr<FPhraseNode> LastVistedNode;

	size_t NodeCount = 0;
};
