// Copyright F-Dudley. All Rights Reserved.


#include "PhraseTree/PhraseEventNode.h"

FPhraseEventNode::FPhraseEventNode() : FPhraseNode()
{

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
    OnPhraseEvent.ExecuteIfBound(InParseRecord);

    return FParseResult(PHRASE_PARSED_AND_EXECUTED);
}
