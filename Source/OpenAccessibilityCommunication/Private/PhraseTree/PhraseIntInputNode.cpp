// Copyright F-Dudley. All Rights Reserved.

#include "PhraseTree/PhraseIntInputNode.h"

#include "PhraseTree/PhraseFloatInputNode.h"

FPhraseIntInputNode::FPhraseIntInputNode(const TCHAR* InInputString)
	: FPhraseInputNode<int32>(InInputString)
{
	
}

FPhraseIntInputNode::FPhraseIntInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes)
	: FPhraseInputNode<int32>(InInputString, InChildNodes)
{
	
}

FPhraseIntInputNode::FPhraseIntInputNode(const TCHAR* InInputString, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes)
	: FPhraseInputNode<int32>(InInputString, InOnPhraseParsed, InChildNodes)
{

}

FPhraseIntInputNode::FPhraseIntInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes, TDelegate<void(int32 Input)> InOnInputRecieved)
	: FPhraseInputNode<int32>(InInputString, InChildNodes, InOnInputRecieved)
{
	
}

FPhraseIntInputNode::FPhraseIntInputNode(const TCHAR* InInputString, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes, TDelegate<void(int32 Input)> InOnInputRecieved)
	: FPhraseInputNode<int32>(InInputString, InOnPhraseParsed, InChildNodes, InOnInputRecieved)
{

}

FPhraseIntInputNode::~FPhraseIntInputNode()
{

}

bool FPhraseIntInputNode::MeetsInputRequirements(const FString& InPhrase)
{
	return InPhrase.IsNumeric() || NumericParser::IsValidNumeric(InPhrase);
}

bool FPhraseIntInputNode::RecordInput(const FString& InInput, FParseRecord& OutParseRecord)
{
	int32 Input = FCString::Atoi(*InInput);

	UParseIntInput* ParseInput = MakeParseInput<UParseIntInput>();
	ParseInput->SetValue(Input);

	OutParseRecord.AddPhraseInput(BoundPhrase, ParseInput);

	OnInputReceived.ExecuteIfBound(Input);

	return true;
}
