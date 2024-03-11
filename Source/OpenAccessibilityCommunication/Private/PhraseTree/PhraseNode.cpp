// Copyright F-Dudley. All Rights Reserved.

#include "PhraseTree/PhraseNode.h"
#include "PhraseTree.h"
#include "OpenAccessibilityComLogging.h"

FPhraseNode::FPhraseNode()
{
    ChildNodes = TArray<TSharedPtr<FPhraseNode>>();
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

bool FPhraseNode::BindChildNode(TSharedPtr<FPhraseNode> InNode)
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

bool FPhraseNode::BindChildrenNodes(TArray<TSharedPtr<FPhraseNode>> InNodes)
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

FParseResult FPhraseNode::ParseChildren(TArray<FString>& InPhraseArray, FParseRecord& InParseRecord)
{
    for (auto& ChildNode : ChildNodes)
    {
        // Cannot have any duplicate Phrases.
        if (ChildNode->RequiresPhrase(InPhraseArray[0]))
        {
            return ChildNode->ParsePhrase(InPhraseArray, InParseRecord);
        }
    }

	return FParseResult(PHRASE_UNABLE_TO_PARSE, AsShared());
}
