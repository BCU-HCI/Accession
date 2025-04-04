// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhraseTree/PhraseNode.h"
#include "PhraseTree/IPhraseContextNode.h"
#include "PhraseTree/Containers/ContextObject.h"

#include "AccessionComLogging.h"
#include "PhraseEventNode.h"

template <class ContextType = UPhraseTreeContextObject>
class FPhraseContextNode : public FPhraseNode, public IPhraseContextNodeBase
{
public:
	FPhraseContextNode(const TCHAR *InInputString)
		: FPhraseNode(InInputString)
	{
		static_assert(std::is_base_of<UPhraseTreeContextObject, ContextType>::value, "ContextType must be a subclass of UPhraseTreeContextObject");

		TPhraseNodeArray EmptyArray = TPhraseNodeArray();
		ConstructContextChildren(EmptyArray);
	}

	FPhraseContextNode(const TCHAR *InInputString, TPhraseNodeArray InChildNodes)
		: FPhraseNode(InInputString, InChildNodes)
	{
		static_assert(std::is_base_of<UPhraseTreeContextObject, ContextType>::value, "ContextType must be a subclass of UPhraseTreeContextObject");

		ConstructContextChildren(InChildNodes);
	}

	FPhraseContextNode(const TCHAR *InInputString, TDelegate<void(FParseRecord &Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes)
		: FPhraseNode(InInputString, InOnPhraseParsed)
	{
		static_assert(std::is_base_of<UPhraseTreeContextObject, ContextType>::value, "ContextType must be a subclass of UPhraseTreeContextObject");

		ConstructContextChildren(InChildNodes);
	}

	virtual ~FPhraseContextNode()
	{
	}

	// FPhraseNode Implementation

	virtual FParseResult ParsePhrase(TArray<FString> &InPhraseWordArray, FParseRecord &InParseRecord) override;

	virtual FParseResult ParsePhraseAsContext(TArray<FString> &InPhraseWordArray, FParseRecord &InParseRecord) override;

	// End FPhraseNode Implementation

protected:
	// FPhraseContextNodeBase Implementation

	virtual bool HasContextObject(TArray<UPhraseTreeContextObject *> InContextObjects) const override;

	virtual UPhraseTreeContextObject *CreateContextObject(FParseRecord &Record) override;

	virtual void ConstructContextChildren(TPhraseNodeArray &InChildNodes) override;

	// End FPhraseContextNodeBase Implementation
};

template <class ContextType>
FParseResult FPhraseContextNode<ContextType>::ParsePhrase(TArray<FString> &InPhraseWordArray, FParseRecord &InParseRecord)
{
	if (!HasContextObject(InParseRecord.GetContextStack()))
	{
		UPhraseTreeContextObject *NewObject = CreateContextObject(InParseRecord);

		InParseRecord.PushContextObj(NewObject);
	}

	return FPhraseNode::ParsePhrase(InPhraseWordArray, InParseRecord);
}

template <class ContextType>
FParseResult FPhraseContextNode<ContextType>::ParsePhraseAsContext(TArray<FString> &InPhraseWordArray, FParseRecord &InParseRecord)
{
	if (!HasContextObject(InParseRecord.GetContextStack()))
	{
		UPhraseTreeContextObject *NewObject = CreateContextObject(InParseRecord);

		InParseRecord.PushContextObj(NewObject);
	}

	if (InPhraseWordArray.IsEmpty())
	{
		UE_LOG(LogAccessionCom, Log, TEXT("|| Emptied Phrase Array ||"))

		return FParseResult(PHRASE_REQUIRES_MORE, AsShared());
	}

	OnPhraseParsed.ExecuteIfBound(InParseRecord);

	// Pass
	return ParseChildren(InPhraseWordArray, InParseRecord);
}

template <class ContextType>
bool FPhraseContextNode<ContextType>::HasContextObject(TArray<UPhraseTreeContextObject *> InContextObjects) const
{
	for (auto &ContextObject : InContextObjects)
	{
		if (ContextObject->IsA(ContextType::StaticClass()) && ContextObject->GetContextRoot() == AsShared())
		{
			return true;
		}
	}

	return false;
}

template <class ContextType>
UPhraseTreeContextObject *FPhraseContextNode<ContextType>::CreateContextObject(FParseRecord &Record)
{
	ContextType *NewContextObject = NewObject<ContextType>();
	NewContextObject->Init();
	NewContextObject->SetContextRootNode(AsShared());

	return NewContextObject;
}

template <class ContextType>
void FPhraseContextNode<ContextType>::ConstructContextChildren(TPhraseNodeArray &InChildNodes)
{
	TSharedPtr<FPhraseEventNode> CloseContextNode = MakeShared<FPhraseEventNode>();
	CloseContextNode->OnPhraseParsed.BindLambda(
		[this](FParseRecord &Record)
		{
			UPhraseTreeContextObject *ContextObject = Record.GetContextObj();
			if (ContextObject->GetContextRoot() == this->AsShared())
			{
				ContextObject->Close();
				ContextObject->RemoveFromRoot();

				Record.PopContextObj();
			}
		});

	this->ChildNodes = TPhraseNodeArray{
		MakeShared<FPhraseNode>(TEXT("CLOSE"),
								TPhraseNodeArray{
									CloseContextNode})};

	this->ChildNodes.Append(InChildNodes);
}