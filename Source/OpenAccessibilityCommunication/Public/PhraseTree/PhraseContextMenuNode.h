// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PhraseTree/PhraseNode.h"
#include "PhraseTree/IPhraseContextNode.h"
#include "PhraseTree/PhraseEventNode.h"
#include "PhraseTree/Containers/ContextMenuObject.h"
#include "OpenAccessibilityComLogging.h"

template<typename ContextMenuType = UPhraseTreeContextMenuObject>
class FPhraseContextMenuNode : public FPhraseNode, public IPhraseContextNodeBase
{
public:

	FPhraseContextMenuNode(const TCHAR* InInputString)
		: FPhraseNode(InInputString)
		, ContextMenuScalar(1.0f)
	{
		static_assert(std::is_base_of<UPhraseTreeContextMenuObject, ContextMenuType>::value, "ContextType must be a subclass of UPhraseTreeContextMenuObject");

		this->ChildNodes = TPhraseNodeArray();
	};

	FPhraseContextMenuNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes)
		: FPhraseNode(InInputString)
		, ContextMenuScalar(1.0f)
	{
		static_assert(std::is_base_of<UPhraseTreeContextMenuObject, ContextMenuType>::value, "ContextType must be a subclass of UPhraseTreeContextMenuObject");

		ConstructContextChildren(InChildNodes);
	};

	FPhraseContextMenuNode(const TCHAR* InInputString, TDelegate<TSharedPtr<IMenu>()> InOnGetMenu, TPhraseNodeArray InChildNodes)
		: FPhraseNode(InInputString)
		, ContextMenuScalar(1.0f)
		, OnGetMenu(InOnGetMenu)
	{
		static_assert(std::is_base_of<UPhraseTreeContextMenuObject, ContextMenuType>::value, "ContextType must be a subclass of UPhraseTreeContextMenuObject");

		ConstructContextChildren(InChildNodes);
	};

	FPhraseContextMenuNode(const TCHAR* InInputString, const float InMenuScalar, TPhraseNodeArray InChildNodes)
		: FPhraseNode(InInputString)
		, ContextMenuScalar(InMenuScalar)
	{
		static_assert(std::is_base_of<UPhraseTreeContextMenuObject, ContextMenuType>::value, "ContextType must be a subclass of UPhraseTreeContextMenuObject");

		ConstructContextChildren(InChildNodes);
	};

	FPhraseContextMenuNode(const TCHAR* InInputString, const float InMenuScalar, TDelegate<TSharedPtr<IMenu>()> InOnGetMenu, TPhraseNodeArray InChildNodes)
		: FPhraseNode(InInputString)
		, ContextMenuScalar(InMenuScalar)
		, OnGetMenu(InOnGetMenu)
	{
		static_assert(std::is_base_of<UPhraseTreeContextMenuObject, ContextMenuType>::value, "ContextType must be a subclass of UPhraseTreeContextMenuObject");

		ConstructContextChildren(InChildNodes);
	}

	FPhraseContextMenuNode(const TCHAR* InInputString, const float InMenuScalar, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes)
		: FPhraseNode(InInputString, InOnPhraseParsed)
		, ContextMenuScalar(InMenuScalar)
	{
		static_assert(std::is_base_of<UPhraseTreeContextMenuObject, ContextMenuType>::value, "ContextType must be a subclass of UPhraseTreeContextMenuObject");

		ConstructContextChildren(InChildNodes);
	}

	FPhraseContextMenuNode(const TCHAR* InInputString, const float InMenuScalar, TDelegate<TSharedPtr<IMenu>()> InOnGetMenu, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes)
		: FPhraseNode(InInputString, InOnPhraseParsed)
		, ContextMenuScalar(InMenuScalar)
		, OnGetMenu(InOnGetMenu)
	{
		static_assert(std::is_base_of<UPhraseTreeContextMenuObject, ContextMenuType>::value, "ContextType must be a subclass of UPhraseTreeContextMenuObject");

		ConstructContextChildren(InChildNodes);
	}

	~FPhraseContextMenuNode()
	{

	}

	// FPhraseNode Implementation

	virtual FParseResult ParsePhrase(TArray<FString>& InPhraseWordArray, FParseRecord& InParseRecord) override;

	// End FPhraseNode Implementation

protected:

	// FPhraseContextNodeBase Implementation

	bool HasContextObject(TArray<UPhraseTreeContextObject*> InContextObjects) const;

	virtual UPhraseTreeContextObject* CreateContextObject();

	virtual void ConstructContextChildren(TPhraseNodeArray& InChildNodes);

	// End FPhraseContextNode Implementation

protected:

	const float ContextMenuScalar;

	TDelegate<TSharedPtr<IMenu>()> OnGetMenu;
};

template<typename ContextMenuType>
FParseResult FPhraseContextMenuNode<ContextMenuType>::ParsePhrase(TArray<FString>& InPhraseWordArray, FParseRecord& InParseRecord)
{
	if (!HasContextObject(InParseRecord.GetContextStack()))
	{
		UPhraseTreeContextObject* NewObject = CreateContextObject();

		InParseRecord.PushContextObj(NewObject);
	}

	return FPhraseNode::ParsePhrase(InPhraseWordArray, InParseRecord);
}

template<typename ContextMenuType>
bool FPhraseContextMenuNode<ContextMenuType>::HasContextObject(TArray<UPhraseTreeContextObject*> InContextObjects) const
{
	for (auto& ContextObject : InContextObjects)
	{
		if (ContextObject->IsA(ContextMenuType::StaticClass()) && ContextObject->GetContextRoot() == AsShared())
		{
			return true;
		}
	}

	return false;
}

template<typename ContextMenuType>
UPhraseTreeContextObject* FPhraseContextMenuNode<ContextMenuType>::CreateContextObject()
{
	if (!OnGetMenu.IsBound())
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("OnGetMenu Delegate Not Bound. Cannot Create Context Object, linked to a Menu."));
		return nullptr;
	}

	TSharedPtr<IMenu> NewMenu = OnGetMenu.Execute();

	if (!NewMenu.IsValid())
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("OnGetMenu Delegate Returned Invalid Menu. Cannot Create Context Object."));
		return nullptr;
	}

	ContextMenuType* NewContextObject = NewObject<ContextMenuType>();
	NewContextObject->Init(NewMenu.ToSharedRef(), this->AsShared());

	NewContextObject->ScaleMenu(ContextMenuScalar);

	return NewContextObject;
}

template<typename ContextMenuType>
void FPhraseContextMenuNode<ContextMenuType>::ConstructContextChildren(TPhraseNodeArray& InChildNodes)
{
	// Construct Context Specific Children Nodes,
	// With Linked Functionality to the Context Menu Object and Root Node.
	TSharedPtr<FPhraseEventNode> CloseContextNode = MakeShared<FPhraseEventNode>();
	CloseContextNode->OnPhraseParsed.BindLambda(
		[this](FParseRecord& Record) {

			UPhraseTreeContextMenuObject* ContextMenu = Record.GetContextObj<UPhraseTreeContextMenuObject>();
			if (ContextMenu->GetContextRoot() == this->AsShared())
			{
				ContextMenu->Close();
				ContextMenu->RemoveFromRoot();

				Record.PopContextObj();
			}
		}
	);

	this->ChildNodes = TPhraseNodeArray{
		MakeShared<FPhraseNode>(TEXT("CLOSE"),
		TPhraseNodeArray {
			CloseContextNode
		})
	};

	this->ChildNodes.Append(InChildNodes);
}