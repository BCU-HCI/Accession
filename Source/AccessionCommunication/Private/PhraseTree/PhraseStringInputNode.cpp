// Copyright F-Dudley. All Rights Reserved.

#include "PhraseTree/PhraseStringInputNode.h"

#include "PhraseTree/Containers/Input/UParseStringInput.h"

FPhraseStringInputNode::FPhraseStringInputNode(const TCHAR* InInputString)
	: FPhraseInputNode(InInputString)
{

};

FPhraseStringInputNode::FPhraseStringInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes)
	: FPhraseInputNode(InInputString, InChildNodes)
{

}

FPhraseStringInputNode::FPhraseStringInputNode(const TCHAR* InInputString, TDelegate<void(FParseRecord& Record)> InOnPhraseParse, TPhraseNodeArray InChildNodes)
	: FPhraseInputNode(InInputString, InOnPhraseParse, InChildNodes)
{

}

FPhraseStringInputNode::FPhraseStringInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes, TDelegate<void(FString Input)> InOnInputRecieved)
	: FPhraseInputNode(InInputString, InChildNodes, InOnInputRecieved)
{

}

FPhraseStringInputNode::~FPhraseStringInputNode()
{

}

bool FPhraseStringInputNode::MeetsInputRequirements(const FString& InPhrase)
{
	if (InPhrase.IsEmpty())
		return false;
	else return true;
}

bool FPhraseStringInputNode::RecordInput(const FString& InInput, FParseRecord& OutParseRecord)
{
	if (InInput.IsEmpty())
		return false;

	UParseStringInput* ParseInput = MakeParseInput<UParseStringInput>();
	ParseInput->SetValue(InInput);

	OutParseRecord.AddPhraseInput(BoundPhrase, ParseInput);

	OnInputReceived.ExecuteIfBound(InInput);

	return true;
}
