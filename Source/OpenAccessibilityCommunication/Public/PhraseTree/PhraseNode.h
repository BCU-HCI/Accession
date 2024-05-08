// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PhraseTree/Containers/ParseResult.h"
#include "PhraseTree/Containers/ParseRecord.h"

class IPhraseNodeBase
{
public:

	/// <summary>
	/// States if the Phrase Node is a LeafNode.
	/// </summary>
	/// <returns>true, if the Node is a Leaf Node otherwise false.</returns>
	virtual bool IsLeafNode() const = 0;

	/// <summary>
	/// Checks if the Given Phrase is Bound to the Node.
	/// </summary>
	/// <param name="InPhrase">The Phrase String to Compare Against.</param>
	/// <returns>True, if the Node requires the given phrase string otherwise false.</returns>
	virtual bool RequiresPhrase(const FString InPhrase) = 0;

	/// <summary>
	/// Parses the phrase down the given Node, propagating down child nodes if required.
	/// </summary>
	/// <param name="InPhraseWordArray">The Array of Phrase Strings to Propogate against.</param>
	/// <param name="InParseRecord">The Record of Propagation of collected context's and inputs.</param>
	/// <returns>Returns the Result of the propogation, including any key nodes met.</returns>
	virtual FParseResult ParsePhrase(TArray<FString>& InPhraseWordArray, FParseRecord& InParseRecord) = 0;

	/// <summary>
	/// Parses the phrase down the given node, propagating down child nodes if required.
	/// Missed Pop of the Phrase Array from this Node.
	/// </summary>
	/// <param name="InPhraseWordArray"></param>
	/// <param name="InParseRecord"></param>
	/// <returns>Returns the Result of the propogation, including any key nodes met.</returns>
	virtual FParseResult ParsePhraseAsContext(TArray<FString>& InPhraseWordArray, FParseRecord& InParseRecord) = 0;
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

	virtual FParseResult ParsePhraseAsContext(TArray<FString>& InPhraseWordArray, FParseRecord& InParseRecord);

	virtual FParseResult ParsePhraseIfRequired(TArray<FString>& InPhraseWordArray, FParseRecord& InParseRecord);

	virtual FParseResult ParseChildren(TArray<FString>& InPhraseArray, FParseRecord& InParseRecord);

	bool CanBindChild(TPhraseNode& InNode);

	bool BindChildNode(TPhraseNode InNode);
	bool BindChildNodeForce(TPhraseNode InNode);

	bool BindChildrenNodes(TPhraseNodeArray InNodes);
	bool BindChildrenNodesForce(TPhraseNodeArray InNodes);

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
