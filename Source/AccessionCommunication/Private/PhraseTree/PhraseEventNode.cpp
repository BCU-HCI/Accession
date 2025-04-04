// Copyright F-Dudley. All Rights Reserved.

#include "PhraseTree/PhraseEventNode.h"
#include "AccessionComLogging.h"

FPhraseEventNode::FPhraseEventNode()
    : FPhraseNode(TEXT("EVENT_NODE"))
{
    OnPhraseParsed = TDelegate<void(FParseRecord &)>();
}

FPhraseEventNode::FPhraseEventNode(TDelegate<void(FParseRecord &)> InEvent)
    : FPhraseNode(TEXT("EVENT_NODE"), InEvent)
{
}

FPhraseEventNode::FPhraseEventNode(TFunction<void(FParseRecord &)> InEventFunction)
    : FPhraseNode(TEXT("EVENT_NODE"), TDelegate<void(FParseRecord &)>::CreateLambda(InEventFunction))
{
}

FPhraseEventNode::~FPhraseEventNode()
{
}

bool FPhraseEventNode::RequiresPhrase(const FString InPhrase)
{
    return true;
}

bool FPhraseEventNode::RequiresPhrase(const FString InPhrase, int32 &OutDistance)
{
    OutDistance = 0;
    return true;
}

FParseResult FPhraseEventNode::ParsePhrase(TArray<FString> &InPhraseArray, FParseRecord &InParseRecord)
{
    if (OnPhraseParsed.ExecuteIfBound(InParseRecord))
    {
        return FParseResult(PHRASE_PARSED_AND_EXECUTED);
    }

    UE_LOG(LogAccessionCom, Warning, TEXT("|| Unable to Execute Event ||"))

    return FParseResult(PHRASE_UNABLE_TO_PARSE, AsShared());
}
