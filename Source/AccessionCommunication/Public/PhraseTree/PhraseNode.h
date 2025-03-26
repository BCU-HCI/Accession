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
	/// States if the the Single Child Node is a Leaf Node, if it exists.
	/// </summary>
	/// <returns></returns>
	virtual bool HasLeafChild() const = 0;

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
	virtual FParseResult ParsePhrase(TArray<FString> &InPhraseWordArray, FParseRecord &InParseRecord) = 0;

	/// <summary>
	/// Parses the phrase down the given node, propagating down child nodes if required.
	/// Missed Pop of the Phrase Array from this Node.
	/// </summary>
	/// <param name="InPhraseWordArray"></param>
	/// <param name="InParseRecord"></param>
	/// <returns>Returns the Result of the propogation, including any key nodes met.</returns>
	virtual FParseResult ParsePhraseAsContext(TArray<FString> &InPhraseWordArray, FParseRecord &InParseRecord) = 0;
};

/**
 *
 */
class ACCESSIONCOMMUNICATION_API FPhraseNode : public TSharedFromThis<FPhraseNode>
{
public:
	FPhraseNode(const TCHAR *InBoundPhrase);
	FPhraseNode(const TCHAR *InBoundPhrase, TDelegate<void(FParseRecord &Record)> InOnPhraseParsed);
	FPhraseNode(const TCHAR *InBoundPhrase, TPhraseNodeArray InChildNodes);
	FPhraseNode(const TCHAR *InBoundPhrase, TDelegate<void(FParseRecord &Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes);

	virtual ~FPhraseNode();

	/// <summary>
	/// Checks if the Node is a Leaf Node.
	/// </summary>
	/// <returns>True, if the Node is a Leaf Node. Otherwise False.</returns>
	virtual bool IsLeafNode() const { return false; }

	virtual bool HasLeafChild() const;

	/// <summary>
	/// Checks if the Node Requires the Given Phrase.
	/// </summary>
	/// <param name="InPhrase">- The Phrase To Check if Required By The Node.</param>
	/// <returns>True, if the Phrase is Required. Otherwise False.</returns>
	virtual bool RequiresPhrase(const FString InPhrase);

	/// <summary>
	/// Checks if the Node Requires the Given Phrase, and Returns the Distance of the Phrase.
	/// </summary>
	/// <param name="InPhrase">- The Phrase To Check if Required By The Node.</param>
	/// <param name="OutDistance">- The Returned Distancing from the Target Phrase To The BoundPhrase.</param>
	/// <returns>True, if the Phrase is Required. Otherwise False.</returns>
	virtual bool RequiresPhrase(const FString InPhrase, int32 &OutDistance);

	/// <summary>
	/// Parses The Phrase Down This Node, Propagating Down Any Child Nodes If Required.
	/// </summary>
	/// <param name="InPhraseWordArray">- The Current Array of Transcription Phrases.</param>
	/// <param name="InParseRecord">- The Parse Record of the Current Propagation.</param>
	/// <returns>The Result of the Parsing of the Phrase, and any Propagation.</returns>
	virtual FParseResult ParsePhrase(TArray<FString> &InPhraseWordArray, FParseRecord &InParseRecord);

	/// <summary>
	/// Parses the Phrase Down This Node, Propagating Down Any Child Nodes If Required.
	/// Does not Pop the Phrase Array.
	/// </summary>
	/// <param name="InPhraseWordArray">- The Current Array of Transcription Phrases.</param>
	/// <param name="InParseRecord">- The Parse Record of the Current Propagation.</param>
	/// <returns>The Result of the Parsing of the Phrase, and any Propagation.</returns>
	virtual FParseResult ParsePhraseAsContext(TArray<FString> &InPhraseWordArray, FParseRecord &InParseRecord);

	/// <summary>
	/// If the Phrase If Required, Parses the Phrase Down This Node, Propagating Down Any Child Nodes If Required.
	/// </summary>
	virtual FParseResult ParsePhraseIfRequired(TArray<FString> &InPhraseWordArray, FParseRecord &InParseRecord);

	/// <summary>
	/// Parses The Children Node of this Node.
	/// </summary>
	/// <param name="InPhraseArray">- The Current Array of Transcription Phrases.</param>
	/// <param name="InParseRecord">- The Parse Record of the Current Propagation.</param>
	/// <returns>The Result of the Parsing of the Phrase, and any Propagation.</returns>
	virtual FParseResult ParseChildren(TArray<FString> &InPhraseArray, FParseRecord &InParseRecord);

	/// <summary>
	/// Checks if the Given Node Can Be Bound as a Child Node.
	/// </summary>
	/// <param name="InNode">- The Node To Check If It Can Be Bound.</param>
	/// <returns>True, if the Node Can Be Bound as a Child. Otherwise False.</returns>
	bool CanBindChild(TPhraseNode &InNode);

	/// <summary>
	/// Binds the Given Node as a Child Node.
	/// </summary>
	/// <param name="InNode">- The Node To Bind as a Child of This Node.</param>
	/// <returns>True, if the Node was Successfully Bound. Otherwise False.</returns>
	bool BindChildNode(TPhraseNode InNode);

	/// <summary>
	/// Forcefully Binds the Given Node as a Child, performing no checks.
	/// </summary>
	/// <param name="InNode">- The Node To Foce Bind as a Child.</param>
	/// <returns>True, if the Node was Successfully Bound. Otherwise False.</returns>
	bool BindChildNodeForce(TPhraseNode InNode);

	/// <summary>
	/// Binds an Array of Nodes as Children of this Node.
	/// </summary>
	/// <param name="InNodes">- The Array of Nodes To Bind as Children.</param>
	/// <returns>True, if the Nodes were Successfully Bound. Otherwise False.</returns>
	bool BindChildrenNodes(TPhraseNodeArray InNodes);

	/// <summary>
	/// Forcefully Binds an Array of Nodes as Children of this Node, performing no checks.
	/// </summary>
	/// <param name="InNodes">- The Array of Nodes To Bind sa Children.</param>
	/// <returns>True, if the Nodes were successfully bound. Otherwise False.</returns>
	bool BindChildrenNodesForce(TPhraseNodeArray InNodes);

protected:
	/// <summary>
	/// Filters through the children, to check if it contains a Leaf Child.
	/// </summary>
	bool HasLeafChild();

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
	TDelegate<void(FParseRecord &Record)> OnPhraseParsed;

protected:
	/// <summary>
	/// Records if the Node has a Leaf Child.
	/// </summary>
	bool bHasLeafChild;
};
