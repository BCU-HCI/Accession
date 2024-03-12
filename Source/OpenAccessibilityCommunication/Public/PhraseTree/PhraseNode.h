// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhraseTree/Containers.h"

typedef TArray<TSharedPtr<FPhraseNode>> FPhraseNodeChildren;

/**
 * 
 */
class OPENACCESSIBILITYCOMMUNICATION_API FPhraseNode : public TSharedFromThis<FPhraseNode>
{
public:

	FPhraseNode(const TCHAR* InBoundPhrase);
	FPhraseNode(const TCHAR* InBoundPhrase, FPhraseNodeChildren InChildNodes);

	virtual ~FPhraseNode();

	virtual bool IsLeafNode() const { return false; }

	virtual bool RequiresPhrase(const FString InPhrase);
	virtual FParseResult ParsePhrase(TArray<FString>& InPhraseWordArray, FParseRecord& InParseRecord);

	virtual bool BindChildNode(TSharedPtr<FPhraseNode> InNode);
	virtual bool BindChildrenNodes(TArray<TSharedPtr<FPhraseNode>> InNodes);

protected:

	virtual FParseResult ParseChildren(TArray<FString>& InPhraseArray, FParseRecord& InParseRecord);

public:

	/// <summary>
	/// The Child Nodes of the Node.
	/// </summary>
	FPhraseNodeChildren ChildNodes;

protected:

	/// <summary>
	/// The Phrase Bound to this
	/// </summary>
	FString BoundPhrase;
};
