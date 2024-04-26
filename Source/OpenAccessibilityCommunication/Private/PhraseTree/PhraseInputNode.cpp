// Copyright F-Dudley. All Rights Reserved.

#include "PhraseTree/PhraseInputNode.h"
#include "PhraseTree/Utils.h"
#include "OpenAccessibilityComLogging.h"

#include "PhraseTree/Containers/Input/UParseIntInput.h"

FPhraseInputNode::FPhraseInputNode(const TCHAR* InInputString) 
    : FPhraseNode(InInputString)
{

}

FPhraseInputNode::FPhraseInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes) 
    : FPhraseNode(InInputString, InChildNodes)
{

}

FPhraseInputNode::FPhraseInputNode(const TCHAR* InInputString, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes)
	: FPhraseNode(InInputString, InOnPhraseParsed, InChildNodes)
{

}

FPhraseInputNode::FPhraseInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes, TDelegate<void(int32 Input)> InOnInputRecieved)
	: FPhraseNode(InInputString, InChildNodes)
{
	OnInputReceived = InOnInputRecieved;
}

FPhraseInputNode::FPhraseInputNode(const TCHAR* InInputString, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes, TDelegate<void(int32 Input)> InOnInputRecieved)
    : FPhraseNode(InInputString, InOnPhraseParsed, InChildNodes)
{
	OnInputReceived = InOnInputRecieved;
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
        if (!InputToRecord.IsNumeric() && NumericParser::IsValidNumeric(InputToRecord, false))
        {
			NumericParser::StringToNumeric(InputToRecord, false);
		}

        if (!RecordInput(InputToRecord, InParseRecord))
        {
            UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Unable to Record Input ||"))

			return FParseResult(PHRASE_UNABLE_TO_PARSE, AsShared());
		}

        OnPhraseParsed.ExecuteIfBound(InParseRecord);

        return ParseChildren(InPhraseArray, InParseRecord);
    }

    return FParseResult(PHRASE_UNABLE_TO_PARSE, AsShared());
}

bool FPhraseInputNode::MeetsInputRequirements(const FString& InPhrase)
{
	return InPhrase.IsNumeric() || NumericParser::IsValidNumeric(InPhrase, false);
}

bool FPhraseInputNode::RecordInput(const FString& InInput, FParseRecord& OutParseRecord)
{
    int32 Input = FCString::Atoi(*InInput);

    UParseIntInput* ParseInput = MakeParseInput<UParseIntInput>();
	ParseInput->SetValue(Input);

    OutParseRecord.AddPhraseInput(BoundPhrase, ParseInput);

    OnInputReceived.ExecuteIfBound(Input);

	return true;
}
