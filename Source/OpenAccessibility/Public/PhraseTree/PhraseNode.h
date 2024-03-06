// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhraseTree/Containers.h"

/**
 * 
 */
class OPENACCESSIBILITY_API FPhraseNode : public TSharedFromThis<FPhraseNode>
{
public:
	FPhraseNode();
	virtual ~FPhraseNode();

	virtual bool IsLeafNode() const { return false; }

	virtual bool RequiresPhrase(const FString InPhrase);
	virtual FParseResult ParsePhrase(TArray<FString>& InPhraseArray, FParseRecord& InParseRecord);

	virtual bool BindChildNode(TSharedPtr<FPhraseNode> InNode);
	virtual bool BindChildrenNodes(TArray<TSharedPtr<FPhraseNode>> InNodes);

protected:

	virtual FParseResult ParseChildren(TArray<FString>& InPhraseArray, FParseRecord& InParseRecord);

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
