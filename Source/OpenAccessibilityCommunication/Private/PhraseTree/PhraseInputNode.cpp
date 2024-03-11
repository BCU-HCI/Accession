// Copyright F-Dudley. All Rights Reserved.

#include "PhraseTree/PhraseInputNode.h"

FPhraseInputNode::FPhraseInputNode() : FPhraseNode()
{

}

FPhraseInputNode::~FPhraseInputNode()
{

}

FParseResult FPhraseInputNode::ParsePhrase(TArray<FString>& InPhraseArray, FParseRecord& InParseRecord)
{
    if (InPhraseArray.Num() == 0)
    {
		return FParseResult(PHRASE_REQUIRES_MORE, AsShared());
	}

    if (InPhraseArray[0].IsNumeric())
    {
        FString InputToRecord = InPhraseArray.Pop();

        InParseRecord.PhraseInputs.Add(InputPhrase, FCString::Atoi(*InputToRecord));

        for (auto& ChildNode : ChildNodes)
        {
            // Cannot have any duplicate Phrases.
            if (ChildNode->RequiresPhrase(InPhraseArray[0]))
            {
                InPhraseArray.Pop();
                return ChildNode->ParsePhrase(InPhraseArray, InParseRecord);
            }
        }
    }

    return FParseResult(PHRASE_UNABLE_TO_PARSE, AsShared());
}
