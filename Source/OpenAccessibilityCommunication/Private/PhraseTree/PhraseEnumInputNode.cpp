// Copyright F-Dudley. All Rights Reserved.


#include "PhraseTree/PhraseEnumInputNode.h"

#include "PhraseTree/Containers/Input/UParseEnumInput.h"

template<typename TEnum>
FPhraseEnumInputNode<TEnum>::FPhraseEnumInputNode(const TCHAR* NodeName) 
	: FPhraseInputNode(NodeName)
{
	static_assert(TIsEnum<TEnum>::Value, "Passed EnumType Must be an Enum.");
};

template<typename TEnum>
FPhraseEnumInputNode<TEnum>::FPhraseEnumInputNode(const TCHAR* NodeName, TPhraseNodeArray InChildNodes)
	: FPhraseInputNode(NodeName, InChildNodes)
{
	static_assert(TIsEnum<TEnum>::Value, "Passed EnumType Must be an Enum");
}

template<typename TEnum>
FPhraseEnumInputNode<TEnum>::FPhraseEnumInputNode(const TCHAR* InInputString, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes)
	: FPhraseInputNode(InInputString, InOnPhraseParsed, InChildNodes)
{
	static_assert(TIsEnum<TEnum>::Value, "Passed EnumType Must be an Enum");
}

template<typename TEnum>
FPhraseEnumInputNode<TEnum>::FPhraseEnumInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes, TDelegate<void(int32 Input)> InOnInputRecieved)
	: FPhraseInputNode(InInputString, InChildNodes, InOnInputRecieved)
{
	static_assert(TIsEnum<TEnum>::Value, "Passed EnumType Must be an Enum");
}

template<typename TEnum>
FPhraseEnumInputNode<TEnum>::FPhraseEnumInputNode(const TCHAR* InInputString, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes, TDelegate<void(int32 Input)> InOnInputRecieved)
	: FPhraseInputNode(InInputString, InOnPhraseParsed, InChildNodes, InOnInputRecieved)
{
	static_assert(TIsEnum<TEnum>::Value, "Passed EnumType Must be an Enum");
}

template<typename TEnum>
FPhraseEnumInputNode<TEnum>::~FPhraseEnumInputNode()
{
	
}

template<typename TEnum>
bool FPhraseEnumInputNode<TEnum>::MeetsInputRequirements(const FString& InPhrase)
{
	UEnum* EnumPtr = StaticEnum<TEnum>();
	if (!EnumPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("FPhraseEnumInputNode::MeetsInputRequirements: EnumPtr is NULL"));
		return false;
	}

	return EnumPtr->IsValidEnumName(*EnumPtr->GenerateFullEnumName(*InPhrase.ToUpper()));
}

template<typename TEnum>
bool FPhraseEnumInputNode<TEnum>::RecordInput(const FString& InInput, FParseRecord& OutParseRecord)
{
	UEnum* EnumPtr = StaticEnum<TEnum>();
	if (!EnumPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("FPhraseEnumInputNode::RecordInput: EnumPtr is NULL"));
		return false;
	}

	int32 Val = EnumPtr->GetValueByNameString(EnumPtr->GenerateFullEnumName(*InInput.ToUpper()));
	if (Val == INDEX_NONE)
	{
		return false;
	}

	UParseEnumInput* ParseInput = MakeParseInput<UParseEnumInput>();
	ParseInput->SetValue(Val);
	ParseInput->SetEnumType(EnumPtr);

	OutParseRecord.AddPhraseInput(BoundPhrase, ParseInput);

	return true;
}