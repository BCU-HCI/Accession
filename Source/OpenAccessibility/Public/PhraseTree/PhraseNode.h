// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class OPENACCESSIBILITY_API FPhraseNode
{
public:
	FPhraseNode();
	~FPhraseNode();

	virtual bool RequiresPhrase(const FString InPhrase);
	virtual FPhrasePropogationResult ParsePhrase(TArray<FString>& InPhraseArray);

	virtual bool BindChildNode(TSharedPtr<FPhraseNode> InNode);
	virtual bool BindChildrenNodes(TArray<TSharedPtr<FPhraseNode>> InNodes);

public:

	/// <summary>
	/// The Child Nodes of the Node.
	/// </summary>
	TArray<TSharedPtr<FPhraseNode>> ChildNodes;

protected:

	/// <summary>
	/// The Phrase Bound to this
	/// </summary>
	const FString BoundPhrase;
};
