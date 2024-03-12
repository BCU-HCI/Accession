// Copyright F-Dudley. All Rights Reserved.

#include "PhraseTree/PhraseInputNode.h"
#include "OpenAccessibilityComLogging.h"

FPhraseInputNode::FPhraseInputNode(const TCHAR* InInputString) : FPhraseNode(InInputString)
{

}

FPhraseInputNode::FPhraseInputNode(const TCHAR* InInputString, FPhraseNodeChildren InChildNodes) : FPhraseNode(InInputString, InChildNodes)
{

}

FPhraseInputNode::~FPhraseInputNode()
{

}

bool FPhraseInputNode::RequiresPhrase(const FString InPhrase)
{
    return InPhrase.IsNumeric();
}

FParseResult FPhraseInputNode::ParsePhrase(TArray<FString>& InPhraseArray, FParseRecord& InParseRecord)
{
    if (InPhraseArray.Num() == 0)
    {
        UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Emptied Phrase Array ||"))

		return FParseResult(PHRASE_REQUIRES_MORE, AsShared());
	}

    if (InPhraseArray.Last().IsNumeric())
    {
        FString InputToRecord = InPhraseArray.Pop();

        InParseRecord.PhraseInputs.Add(BoundPhrase, FCString::Atoi(*InputToRecord));

        return ParseChildren(InPhraseArray, InParseRecord);
    }

    return FParseResult(PHRASE_UNABLE_TO_PARSE, AsShared());
}
