// Copyright F-Dudley. All Rights Reserved.


#include "PhraseTree/PhraseEventNode.h"
#include "OpenAccessibilityComLogging.h"

FPhraseEventNode::FPhraseEventNode() : FPhraseNode(TEXT("EVENT_NODE"))
{
    OnPhraseParsed = TDelegate<void(const FParseRecord&)>();
}

FPhraseEventNode::FPhraseEventNode(TDelegate<void(const FParseRecord&)> InEvent) : FPhraseNode(TEXT("EVENT_NODE"))
{
    OnPhraseParsed = InEvent;
}

FPhraseEventNode::~FPhraseEventNode()
{

}

bool FPhraseEventNode::RequiresPhrase(const FString InPhrase)
{
    return true;
}

FParseResult FPhraseEventNode::ParsePhrase(TArray<FString>& InPhraseArray, FParseRecord& InParseRecord)
{
    if (OnPhraseParsed.ExecuteIfBound(InParseRecord))
    {
        return FParseResult(PHRASE_PARSED_AND_EXECUTED);
    }

    UE_LOG(LogOpenAccessibilityCom, Warning, TEXT("|| Unable to Execute Event ||"))

    return FParseResult(PHRASE_UNABLE_TO_PARSE, AsShared());
}
