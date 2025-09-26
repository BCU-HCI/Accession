// Copyright (C) HCI-BCU. All rights reserved.
// Published under GPLv3 License, 2025. See LICENSE in the Plugin Root for more information.

#include "PhraseTree/PhraseFloatInputNode.h"

FPhraseFloatInputNode::FPhraseFloatInputNode(const TCHAR* InInputString)
	: FPhraseInputNode<float>(InInputString)
{

}

FPhraseFloatInputNode::FPhraseFloatInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes)
	: FPhraseInputNode<float>(InInputString, InChildNodes)
{

}

FPhraseFloatInputNode::FPhraseFloatInputNode(const TCHAR* InInputString, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes)
	: FPhraseInputNode<float>(InInputString, InOnPhraseParsed, InChildNodes)
{

}

FPhraseFloatInputNode::FPhraseFloatInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes, TDelegate<void(float Input)> InOnInputRecieved)
	: FPhraseInputNode<float>(InInputString, InChildNodes, InOnInputRecieved)
{

}

FPhraseFloatInputNode::FPhraseFloatInputNode(const TCHAR* InInputString, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes, TDelegate<void(float Input)> InOnInputRecieved)
	: FPhraseInputNode<float>(InInputString, InOnPhraseParsed, InChildNodes, InOnInputRecieved)
{

}

FPhraseFloatInputNode::~FPhraseFloatInputNode()
{

}

bool FPhraseFloatInputNode::MeetsInputRequirements(const FString& InPhrase)
{
	return InPhrase.IsNumeric();
}

bool FPhraseFloatInputNode::RecordInput(const FString& InInput, FParseRecord& OutParseRecord)
{
	float Input = FCString::Atof(*InInput);

	UParseFloatInput* ParseInput = MakeParseInput<UParseFloatInput>();
	ParseInput->SetValue(Input);

	OutParseRecord.AddPhraseInput(BoundPhrase, ParseInput);

	OnInputReceived.ExecuteIfBound(Input);

	return true;
}
