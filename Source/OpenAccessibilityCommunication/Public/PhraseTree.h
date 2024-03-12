// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhraseTree/PhraseNode.h"
#include "PhraseTree/Containers.h"


enum EPhraseTreeBranchBindResult : uint8_t
{
	/// <summary>
	/// test
	/// </summary>
	BRANCH_NOT_BOUND,

	/// <summary>
	/// test
	/// </summary>
	BRANCH_BOUND,
	BRANCH_SPLIT
};

struct OPENACCESSIBILITYCOMMUNICATION_API FPhraseTreeBranchBind
{
	FPhraseTreeBranchBind()
	{

	}

	FPhraseTreeBranchBind(TPhraseNode InRootNode, TPhraseNode InBranchRoot)
	{
		StartNode = InRootNode;
		BranchRoot = InBranchRoot;
	}

	~FPhraseTreeBranchBind()
	{
		StartNode.Reset();
		BranchRoot.Reset();
	}

	/// <summary>
	/// The Node to start the binding of this branch root.
	/// </summary>
	TPhraseNode StartNode;

	/// <summary>
	/// The Root Node of the Branch that needs to be bound.
	/// </summary>
	TPhraseNode BranchRoot;
};

/**
 * 
 */
class OPENACCESSIBILITYCOMMUNICATION_API FPhraseTree : public FPhraseNode
{
public:
	FPhraseTree();
	~FPhraseTree();

	// FPhaseNode Implementation
	virtual FParseResult ParsePhrase(TArray<FString>& InPhraseWordArray, FParseRecord& InParseRecord) override;
	// End FPhaseNode Implementation

	/// <summary>
	/// Bind a branch to the tree.
	/// Attaching to any overlapping nodes.
	/// </summary>
	/// <param name="InNode">The constructed branch to attach to the tree.</param>
	void BindBranch(const TPhraseNode& InNode);
	
	/// <summary>
	/// Bind Multiple Branches to the Tree, that are not connected.
	/// </summary>
	void BindBranches(const TPhraseNodeArray& InNodes);

	void ParseTranscription(TArray<FString> InTranscriptionSegments);

private:

	/// <summary>
	/// The Node that was last visited in the tree.
	/// Allowing for quick entry, due to split phrases.
	/// </summary>
	TSharedPtr<FPhraseNode> LastVistedNode;
};
