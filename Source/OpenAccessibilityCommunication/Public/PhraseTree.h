// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhraseTree/PhraseNode.h"
#include "PhraseTree/Containers.h"


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
	void BindBranch(const TSharedPtr<FPhraseNode> InNode);
	
	/// <summary>
	/// Bind Multiple Branches to the Tree, that are not connected.
	/// </summary>
	void BindBranches(const TArray<TSharedPtr<FPhraseNode>> InNodes);

	void ParseTranscription(TArray<FString> InTranscriptionSegments);


private:

	void ConstructPhraseTree();

private:

	/// <summary>
	/// The Node that was last visited in the tree.
	/// Allowing for quick entry, due to split phrases.
	/// </summary>
	TSharedPtr<FPhraseNode> LastVistedNode;
};
