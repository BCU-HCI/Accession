// Copyright F-Dudley. All Rights Reserved.

#include "PhraseTree/PhraseInputNode.h"
#include "PhraseTree/Utils.h"
#include "OpenAccessibilityComLogging.h"

#include "PhraseTree/Containers/Input/UParseIntInput.h"

template<typename InputType>
FPhraseInputNode<InputType>::FPhraseInputNode(const TCHAR* InInputString) 
    : FPhraseNode(InInputString)
{

}

template<typename InputType>
FPhraseInputNode<InputType>::FPhraseInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes) 
    : FPhraseNode(InInputString, InChildNodes)
{

}

template<typename InputType>
FPhraseInputNode<InputType>::FPhraseInputNode(const TCHAR* InInputString, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes)
	: FPhraseNode(InInputString, InOnPhraseParsed, InChildNodes)
{

}

template<typename InputType>
FPhraseInputNode<InputType>::FPhraseInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes, TDelegate<void(InputType Input)> InOnInputRecieved)
	: FPhraseNode(InInputString, InChildNodes)
{
	OnInputReceived = InOnInputRecieved;
}

template<typename InputType>
FPhraseInputNode<InputType>::FPhraseInputNode(const TCHAR* InInputString, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes, TDelegate<void(InputType Input)> InOnInputRecieved)
    : FPhraseNode(InInputString, InOnPhraseParsed, InChildNodes)
{
	OnInputReceived = InOnInputRecieved;
}

template<typename InputType>
FPhraseInputNode<InputType>::~FPhraseInputNode()
{

}

template<typename InputType>
bool FPhraseInputNode<InputType>::RequiresPhrase(const FString InPhrase)
{
    return MeetsInputRequirements(InPhrase);
}

template <typename InputType>
bool FPhraseInputNode<InputType>::RequiresPhrase(const FString InPhrase, int32& OutDistance) 
{
    bool bMeetsRequirements = MeetsInputRequirements(InPhrase);
    OutDistance = bMeetsRequirements ? 0 : INT32_MAX;

    return bMeetsRequirements;
}

template<typename InputType>
FParseResult FPhraseInputNode<InputType>::ParsePhrase(TArray<FString>& InPhraseArray, FParseRecord& InParseRecord)
{
    if (InPhraseArray.Num() == 0)
    {
        UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Emptied Phrase Array ||"))

		return FParseResult(PHRASE_REQUIRES_MORE, AsShared());
	}

    if (MeetsInputRequirements(InPhraseArray.Last()))
    {
        // Get the Input String.
        FString InputToRecord = InPhraseArray.Pop();

        // Append the Input String to the Record.
        InParseRecord.AddPhraseString(InputToRecord);

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

template<typename InputType>
bool FPhraseInputNode<InputType>::MeetsInputRequirements(const FString& InPhrase)
{
	return InPhrase.IsNumeric() || NumericParser::IsValidNumeric(InPhrase, false);
}

template<typename InputType>
bool FPhraseInputNode<InputType>::RecordInput(const FString& InInput, FParseRecord& OutParseRecord)
{
	return false;
}

bool FPhraseInputNode<int32>::RecordInput(const FString& InInput, FParseRecord& OutParseRecord)
{
    int32 Input = FCString::Atoi(*InInput);

    UParseIntInput* ParseInput = MakeParseInput<UParseIntInput>();
	ParseInput->SetValue(Input);

    OutParseRecord.AddPhraseInput(BoundPhrase, ParseInput);

    OnInputReceived.ExecuteIfBound(Input);

	return true;
}
