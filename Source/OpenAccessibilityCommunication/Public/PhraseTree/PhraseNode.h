// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhraseTree/Containers.h"

/**
 * 
 */
class OPENACCESSIBILITYCOMMUNICATION_API FPhraseNode : public TSharedFromThis<FPhraseNode>
{
public:

	FPhraseNode(const TCHAR* InBoundPhrase);
	FPhraseNode(const TCHAR* InBoundPhrase, TPhraseNodeArray InChildNodes);

	virtual ~FPhraseNode();

	virtual bool IsLeafNode() const { return false; }

	virtual bool RequiresPhrase(const FString InPhrase);
	virtual FParseResult ParsePhrase(TArray<FString>& InPhraseWordArray, FParseRecord& InParseRecord);

	virtual FParseResult ParsePhraseIfRequired(TArray<FString>& InPhraseWordArray, FParseRecord& InParseRecord);

	bool CanBindChild(TPhraseNode& InNode);

	bool BindChildNode(TPhraseNode InNode);
	bool BindChildNodeForce(TPhraseNode InNode);

	bool BindChildrenNodes(TPhraseNodeArray InNodes);
	bool BindChildrenNodesForce(TPhraseNodeArray InNodes);

protected:

	virtual FParseResult ParseChildren(TArray<FString>& InPhraseArray, FParseRecord& InParseRecord);

public:

	/// <summary>
	/// This Nodes Parent Node.
	/// </summary>
	TWeakPtr<FPhraseNode> ParentNode;

	/// <summary>
	/// The Child Nodes of the Node.
	/// </summary>
	TPhraseNodeArray ChildNodes;

	/// <summary>
	/// The Phrase Bound to this
	/// </summary>
	FString BoundPhrase;

protected:
};
