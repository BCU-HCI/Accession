// Copyright F-Dudley. All Rights Reserved.

#include "PhraseTree/PhraseNode.h"
#include "PhraseTree.h"
#include "OpenAccessibilityLogging.h"

FPhraseNode::FPhraseNode()
{

}

FPhraseNode::~FPhraseNode()
{

}

bool FPhraseNode::RequiresPhrase(FString InPhrase)
{
    return InPhrase.Equals(BoundPhrase, ESearchCase::IgnoreCase);
}

FPhrasePropogationResult FPhraseNode::ParsePhrase(TArray<FString>& InPhraseArray)
{
    if (InPhraseArray.IsEmpty())
    {
        UE_LOG(LogOpenAccessibility, Log, TEXT("|| Emptied Phrase Array ||"))

        return FPhrasePropogationResult(PHRASE_CANNOT_BE_PARSED, MakeShared<FPhraseNode>(this).ToWeakPtr());
    }
    
    // Loop through the Child Nodes to see if any require the next Phrase.
    for (auto& ChildNode : ChildNodes)
    {
        // Cannot have any duplicate Phrases.
        if (ChildNode->RequiresPhrase(InPhraseArray[0]))
        {
			return ChildNode->ParsePhrase(InPhraseArray);
		}
	}
    
    // Return if no Child Nodes require the next Phrase.
    return FPhrasePropogationResult(PHRASE_CANNOT_BE_PARSED);
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
