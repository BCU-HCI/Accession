// Copyright F-Dudley. All Rights Reserved.

#include "PhraseTree/PhraseContextNode.h"

#include <type_traits>

template<class ContextType>
inline FPhraseContextNode<ContextType>::FPhraseContextNode(const TCHAR* InInputString)
	: FPhraseNode(InInputString)
{
	static_assert(std::is_base_of<UPhraseTreeContextObject, ContextType>::value, "ContextType must be a subclass of UPhraseTreeContextObject");

	TPhraseNodeArray EmptyArray = TPhraseNodeArray();
	ConstructContextChildren(EmptyArray);
}

template<class ContextType>
FPhraseContextNode<ContextType>::FPhraseContextNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes)
	: FPhraseNode(InInputString, InChildNodes)
{
	static_assert(std::is_base_of<UPhraseTreeContextObject, ContextType>::value, "ContextType must be a subclass of UPhraseTreeContextObject");

	ConstructContextChildren(InChildNodes);
}

template<class ContextType>
FPhraseContextNode<ContextType>::FPhraseContextNode(const TCHAR* InInputString, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes)
	: FPhraseNode(InInputString, InOnPhraseParsed)
{
	static_assert(std::is_base_of<UPhraseTreeContextObject, ContextType>::value, "ContextType must be a subclass of UPhraseTreeContextObject");

	ConstructContextChildren(InChildNodes);
}

template<class ContextType>
FPhraseContextNode<ContextType>::~FPhraseContextNode()
{

}

template<class ContextType>
FParseResult FPhraseContextNode<ContextType>::ParsePhrase(TArray<FString>& InPhraseWordArray, FParseRecord& InParseRecord)
{
	if (!HasContextObject(InParseRecord.GetContextStack()))
	{
		UPhraseTreeContextObject* NewObject = CreateContextObject();

		InParseRecord.PushContextObj(NewObject);
	}

	return FPhraseNode::ParsePhrase(InPhraseWordArray, InParseRecord);
}

template<class ContextType>
bool FPhraseContextNode<ContextType>::HasContextObject(TArray<UPhraseTreeContextObject*> InContextObjects) const
{
	for (auto& ContextObject : InContextObjects)
	{
		if (ContextObject->IsA(ContextType::StaticClass()) && ContextObject->GetContextRoot() == AsShared())
		{
			return true;
		}
	}

	return false;
}


template<class ContextType>
UPhraseTreeContextObject* FPhraseContextNode<ContextType>::CreateContextObject()
{
	ContextType* NewContextObject = NewObject<ContextType>();
	NewContextObject->SetContextRootNode(AsShared());

	return NewContextObject;
}

template<class ContextType>
void FPhraseContextNode<ContextType>::ConstructContextChildren(TPhraseNodeArray& InChildNodes)
{
	this->ChildNodes = InChildNodes;
}
