// Copyright (C) HCI-BCU 2025. All rights reserved.

#include "PhraseTree/PhraseNode.h"
#include "PhraseTree.h"
#include "AccessionComLogging.h"
#include "AccessionCommunicationSettings.h"

#include "Algo/LevenshteinDistance.h"

FPhraseNode::FPhraseNode(const TCHAR *InBoundPhrase)
{
    BoundPhrase = InBoundPhrase;
    BoundPhrase.ToUpperInline();

    ChildNodes = TArray<TSharedPtr<FPhraseNode>>();
}

FPhraseNode::FPhraseNode(const TCHAR *InBoundPhrase, TDelegate<void(FParseRecord &Record)> InOnPhraseParsed)
{
    BoundPhrase = InBoundPhrase;
    BoundPhrase.ToUpperInline();

    OnPhraseParsed = InOnPhraseParsed;
    ChildNodes = TArray<TSharedPtr<FPhraseNode>>();
}

FPhraseNode::FPhraseNode(const TCHAR *InBoundPhrase, TPhraseNodeArray InChildNodes)
{
    BoundPhrase = InBoundPhrase;
    BoundPhrase.ToUpperInline();

    ChildNodes = InChildNodes;
}

FPhraseNode::FPhraseNode(const TCHAR *InBoundPhrase, TDelegate<void(FParseRecord &Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes)
{
    BoundPhrase = InBoundPhrase;
    BoundPhrase.ToUpperInline();

    OnPhraseParsed = InOnPhraseParsed;
    ChildNodes = InChildNodes;
}

FPhraseNode::~FPhraseNode()
{
}

bool FPhraseNode::HasLeafChild() const
{
    return bHasLeafChild;
}

bool FPhraseNode::RequiresPhrase(FString InPhrase)
{
    return InPhrase.Equals(BoundPhrase, ESearchCase::IgnoreCase) || Algo::LevenshteinDistance(BoundPhrase, InPhrase) < 3;
}

bool FPhraseNode::RequiresPhrase(const FString InPhrase, int32 &OutDistance)
{
	const UAccessionCommunicationSettings* ACSettings = GetDefault<UAccessionCommunicationSettings>();

    OutDistance = Algo::LevenshteinDistance(BoundPhrase, InPhrase);

    return InPhrase.Equals(BoundPhrase, ESearchCase::IgnoreCase) || OutDistance <= (ACSettings != nullptr ? ACSettings->CommandConfidenceThreshold : 2);
}

FParseResult FPhraseNode::ParsePhrase(TArray<FString> &InPhraseArray,
                                      FParseRecord &InParseRecord)
{
    if (InPhraseArray.IsEmpty())
    {
        UE_LOG(LogAccessionCom, Log, TEXT("|| Emptied Phrase Array ||"))

        return FParseResult(PHRASE_REQUIRES_MORE, AsShared());
    }

    // Pop the Phrase Linked to this Node.
    // Apply to the Record.
    FString LinkedPhrase = InPhraseArray.Pop();

    // Append Removed Phrase To Record.
    InParseRecord.AddPhraseString(LinkedPhrase);

    OnPhraseParsed.ExecuteIfBound(InParseRecord);

    // Pass
    return ParseChildren(InPhraseArray, InParseRecord);
}

FParseResult FPhraseNode::ParsePhraseAsContext(TArray<FString> &InPhraseWordArray, FParseRecord &InParseRecord)
{
    if (InPhraseWordArray.IsEmpty())
    {
        UE_LOG(LogAccessionCom, Log, TEXT("|| Emptied Phrase Array ||"))

        return FParseResult(PHRASE_REQUIRES_MORE, AsShared());
    }

    OnPhraseParsed.ExecuteIfBound(InParseRecord);

    return ParseChildren(InPhraseWordArray, InParseRecord);
}

FParseResult FPhraseNode::ParsePhraseIfRequired(TArray<FString> &InPhraseWordArray, FParseRecord &InParseRecord)
{
    if (RequiresPhrase(InPhraseWordArray.Last()))
    {
        return ParsePhrase(InPhraseWordArray, InParseRecord);
    }

    return FParseResult(PHRASE_UNABLE_TO_PARSE);
}

bool FPhraseNode::CanBindChild(TPhraseNode &InNode)
{
    for (auto &ChildNode : ChildNodes)
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

    for (auto &ChildNode : ChildNodes)
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
    for (auto &InNode : InNodes)
    {
        for (auto &ChildNode : ChildNodes)
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
    for (auto &InNode : InNodes)
    {
        ChildNodes.AddUnique(InNode);
    }

    return true;
}

bool FPhraseNode::HasLeafChild()
{
    return ChildNodes.Num() == 1 && ChildNodes[0]->IsLeafNode();
}

FParseResult FPhraseNode::ParseChildren(TArray<FString> &InPhraseArray, FParseRecord &InParseRecord)
{
    if (HasLeafChild())
        return ChildNodes[0]->ParsePhrase(InPhraseArray, InParseRecord);
    if (InPhraseArray.IsEmpty())
        return FParseResult(PHRASE_REQUIRES_MORE, AsShared());

    // Below Can Be Optimized.
    // Maybe bypass the loop if Distance == 0 and Sort ChildNodes with Derrived PhraseNodes Last?

    int FoundChildIndex = -1;
    {
        int32 FoundChildDistance = INT32_MAX, CurrentDistance = INT32_MAX;

        for (int i = 0; i < ChildNodes.Num(); i++)
        {
            // Child Nodes Require Unique Phrases to Siblings.
            if (ChildNodes[i]->RequiresPhrase(InPhraseArray.Last(), CurrentDistance))
            {
                if (FoundChildDistance > CurrentDistance)
                {
                    FoundChildIndex = i;
                    FoundChildDistance = CurrentDistance;
                }
            }
        }
    }

    if (FoundChildIndex != -1)
    {
        return ChildNodes[FoundChildIndex]->ParsePhrase(InPhraseArray, InParseRecord);
    }

    /*else if (!InPhraseArray.IsEmpty())
    {
        return FParseResult(PHRASE_REQUIRES_MORE_CORRECT_PHRASES, AsShared());
    }*/

    return FParseResult(PHRASE_UNABLE_TO_PARSE, AsShared());
}
