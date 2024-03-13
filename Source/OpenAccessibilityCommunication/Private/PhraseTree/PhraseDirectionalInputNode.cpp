// Copyright F-Dudley. All Rights Reserved.


#include "PhraseTree/PhraseDirectionalInputNode.h"
#include "PhraseTree/InputContainers.h"

FPhraseDirectionalInputNode::FPhraseDirectionalInputNode(const TCHAR* InInputString) : FPhraseInputNode(InInputString)
{
}

FPhraseDirectionalInputNode::FPhraseDirectionalInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes) 
	: FPhraseInputNode(InInputString, InChildNodes)
{

}

FPhraseDirectionalInputNode::~FPhraseDirectionalInputNode()
{

}

bool FPhraseDirectionalInputNode::MeetsInputRequirements(const FString& InPhrase)
{
	UEnum* EnumPtr = StaticEnum<EPhraseDirectionalInput>();
	if (!EnumPtr)
	{
		return false;
	}

	return EnumPtr->IsValidEnumName(*EnumPtr->GenerateFullEnumName(*InPhrase));
}

bool FPhraseDirectionalInputNode::RecordInput(const FString& InInput, FParseRecord& OutParseRecord)
{
	UEnum* EnumPtr = StaticEnum<EPhraseDirectionalInput>();
	if (!EnumPtr)
	{
		return false;
	}

	int32 Val = EnumPtr->GetValueByNameString(EnumPtr->GenerateFullEnumName(*InInput));
	if (Val == INDEX_NONE)
	{
		return false;
	}

	OutParseRecord.PhraseInputs.Add(BoundPhrase, Val);

	return true;
}
