// Copyright F-Dudley. All Rights Reserved.

#include "PhraseTree/PhraseInputNode.h"
#include "OpenAccessibilityComLogging.h"

FPhraseInputNode::FPhraseInputNode(const TCHAR* InInputString) : FPhraseNode(InInputString)
{

}

FPhraseInputNode::FPhraseInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes) : FPhraseNode(InInputString, InChildNodes)
{

}

FPhraseInputNode::~FPhraseInputNode()
{

}

bool FPhraseInputNode::RequiresPhrase(const FString InPhrase)
{
    return MeetsInputRequirements(InPhrase);
}

FParseResult FPhraseInputNode::ParsePhrase(TArray<FString>& InPhraseArray, FParseRecord& InParseRecord)
{
    if (InPhraseArray.Num() == 0)
    {
        UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Emptied Phrase Array ||"))

		return FParseResult(PHRASE_REQUIRES_MORE, AsShared());
	}

    if (MeetsInputRequirements(InPhraseArray.Last()))
    {
        FString InputToRecord = InPhraseArray.Pop();

        if (!RecordInput(InputToRecord, InParseRecord))
        {
            UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Unable to Record Input ||"))

			return FParseResult(PHRASE_UNABLE_TO_PARSE, AsShared());
		}

        return ParseChildren(InPhraseArray, InParseRecord);
    }

    return FParseResult(PHRASE_UNABLE_TO_PARSE, AsShared());
}

bool FPhraseInputNode::MeetsInputRequirements(const FString& InPhrase)
{
	return InPhrase.IsNumeric();
}

bool FPhraseInputNode::RecordInput(const FString& InInput, FParseRecord& OutParseRecord)
{
	OutParseRecord.PhraseInputs.Add(BoundPhrase, FCString::Atoi(*InInput));

	return true;
}
