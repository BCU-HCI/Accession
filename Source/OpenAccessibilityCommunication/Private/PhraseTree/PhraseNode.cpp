// Copyright F-Dudley. All Rights Reserved.

#include "PhraseTree/PhraseNode.h"
#include "PhraseTree.h"
#include "OpenAccessibilityComLogging.h"

FPhraseNode::FPhraseNode(const TCHAR* InBoundPhrase)
{
	BoundPhrase = InBoundPhrase;
	ChildNodes = TArray<TSharedPtr<FPhraseNode>>();
}

FPhraseNode::FPhraseNode(const TCHAR* InBoundPhrase, TPhraseNodeArray InChildNodes)
{
	BoundPhrase = InBoundPhrase;
	ChildNodes = InChildNodes;
}

FPhraseNode::~FPhraseNode()
{

}

bool FPhraseNode::RequiresPhrase(FString InPhrase)
{
    return InPhrase.Equals(BoundPhrase, ESearchCase::IgnoreCase);
}

FParseResult FPhraseNode::ParsePhrase(TArray<FString>& InPhraseArray, FParseRecord& InParseRecord)
{
    if (InPhraseArray.IsEmpty())
    {
        UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Emptied Phrase Array ||"))

        return FParseResult(PHRASE_REQUIRES_MORE, AsShared());
    }

    // Pop the Phrase Linked to this Node.
    FString PhraseToParse = InPhraseArray.Pop();
    
    // Pass 
    return ParseChildren(InPhraseArray, InParseRecord);
}

FParseResult FPhraseNode::ParsePhraseIfRequired(TArray<FString>& InPhraseWordArray, FParseRecord& InParseRecord)
{
    if (RequiresPhrase(InPhraseWordArray.Last()))
    {
		return ParsePhrase(InPhraseWordArray, InParseRecord);
	}

    return FParseResult(PHRASE_UNABLE_TO_PARSE);
}

bool FPhraseNode::CanBindChild(TPhraseNode& InNode)
{
    for (auto& ChildNode : ChildNodes)
    {
        if (ChildNode->RequiresPhrase(InNode->BoundPhrase) || ChildNode->IsLeafNode())
        {
			return false;
		}
	}

    return true;
}

bool FPhraseNode::BindChildNode(TPhraseNode InNode)
{
    if (!InNode.IsValid())
        return false;
	
    for (auto& ChildNode : ChildNodes)
    {
        if (ChildNode->RequiresPhrase(InNode->BoundPhrase))
        {
            return ChildNode->BindChildrenNodes(InNode->ChildNodes);
		}
        else
        {
            ChildNodes.AddUnique(ChildNode);
            return true;
        }
    }

    return false;
}

bool FPhraseNode::BindChildNodeForce(TPhraseNode InNode)
{
    ChildNodes.AddUnique(InNode);

    return true;
}

bool FPhraseNode::BindChildrenNodes(TPhraseNodeArray InNodes)
{
    for (auto& InNode : InNodes)
    {
        for (auto& ChildNode : ChildNodes)
        {
            if (ChildNode->RequiresPhrase(InNode->BoundPhrase))
            {
				return ChildNode->BindChildrenNodes(InNode->ChildNodes);
			}
            else
            {
				ChildNodes.AddUnique(ChildNode);
				return true;
			}
		}
    }

    return false;
}

bool FPhraseNode::BindChildrenNodesForce(TPhraseNodeArray InNodes)
{
    for (auto& InNode : InNodes)
    {
		ChildNodes.AddUnique(InNode);
	}

    return true;
}

FParseResult FPhraseNode::ParseChildren(TArray<FString>& InPhraseArray, FParseRecord& InParseRecord)
{
    for (auto& ChildNode : ChildNodes)
    {
        // ChildNodes cannot have duplicate bound phrases.
        if (ChildNode->RequiresPhrase(InPhraseArray.Last()) || ChildNode->IsLeafNode())
        {
            return ChildNode->ParsePhrase(InPhraseArray, InParseRecord);
        }
    }

	return FParseResult(PHRASE_UNABLE_TO_PARSE, AsShared());
}
