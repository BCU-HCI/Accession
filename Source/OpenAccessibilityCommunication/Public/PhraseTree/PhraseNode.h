// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PhraseTree/Containers/ParseResult.h"
#include "PhraseTree/Containers/ParseRecord.h"

class IPhraseNodeBase
{
public:

	virtual bool IsLeafNode() const = 0;

	virtual bool RequiresPhrase(const FString InPhrase) = 0;

	virtual FParseResult ParsePhrase(TArray<FString>& InPhraseWordArray, FParseRecord& InParseRecord) = 0;
};

/**
 * 
 */
class OPENACCESSIBILITYCOMMUNICATION_API FPhraseNode :  public TSharedFromThis<FPhraseNode>
{
public:

	FPhraseNode(const TCHAR* InBoundPhrase);
	FPhraseNode(const TCHAR* InBoundPhrase, TDelegate<void (FParseRecord& Record)> InOnPhraseParsed);
	FPhraseNode(const TCHAR* InBoundPhrase, TPhraseNodeArray InChildNodes);
	FPhraseNode(const TCHAR* InBoundPhrase, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes);

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

	// Phrase To Be Executed On the Parse Command
	TDelegate<void (FParseRecord& Record)> OnPhraseParsed;

protected:
};
