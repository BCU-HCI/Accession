// Copyright F-Dudley. All Rights Reserved.

#include "PhraseTree/PhraseContextMenuNode.h"
#include "PhraseTree/PhraseEventNode.h"

/*
template<typename ContextMenuType>
inline FPhraseContextMenuNode<ContextMenuType>::FPhraseContextMenuNode(const TCHAR* InInputString)
	: FPhraseNode(InInputString)
	, ContextMenuScalar(1.0f)
{
	static_assert(std::is_base_of<UPhraseTreeContextMenuObject, ContextMenuType>::value, "ContextType must be a subclass of UPhraseTreeContextMenuObject");

	this->ChildNodes = TPhraseNodeArray{
		
	};
}

template<typename ContextMenuType>
FORCEINLINE FPhraseContextMenuNode<ContextMenuType>::FPhraseContextMenuNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes)
	: FPhraseNode(InInputString)
	, ContextMenuScalar(1.0f)
{
	static_assert(std::is_base_of<UPhraseTreeContextMenuObject, ContextMenuType>::value, "ContextType must be a subclass of UPhraseTreeContextMenuObject");

	ConstructContextChildren(InChildNodes);
}

template<typename ContextMenuType>
inline FPhraseContextMenuNode<ContextMenuType>::FPhraseContextMenuNode(const TCHAR* InInputString, const float MenuScalar, TPhraseNodeArray InChildNodes)
	: FPhraseNode(InInputString)
	, ContextMenuScalar(MenuScalar)
{
	static_assert(std::is_base_of<UPhraseTreeContextMenuObject, ContextMenuType>::value, "ContextType must be a subclass of UPhraseTreeContextMenuObject");

	ConstructContextChildren(InChildNodes);
}

template<typename ContextMenuType>
inline FPhraseContextMenuNode<ContextMenuType>::FPhraseContextMenuNode(const TCHAR* InInputString, TDelegate<TSharedPtr<IMenu>()> InOnGetMenu, TPhraseNodeArray InChildNodes)
	: FPhraseNode(InInputString)
	, ContextMenuScalar(1.0f)
	, OnGetMenu(InOnGetMenu)
{
	static_assert(std::is_base_of<UPhraseTreeContextMenuObject, ContextMenuType>::value, "ContextType must be a subclass of UPhraseTreeContextMenuObject");
	
	ConstructContextChildren(InChildNodes);
}

template<typename ContextMenuType>
inline FPhraseContextMenuNode<ContextMenuType>::FPhraseContextMenuNode(const TCHAR* InInputString, const float MenuScalar, TDelegate<TSharedPtr<IMenu>()> InOnGetMenu, TPhraseNodeArray InChildNodes)
	: FPhraseNode(InInputString)
	, ContextMenuScalar(MenuScalar)
	, OnGetMenu(InOnGetMenu)
{
	static_assert(std::is_base_of<UPhraseTreeContextMenuObject, ContextMenuType>::value, "ContextType must be a subclass of UPhraseTreeContextMenuObject");

	ConstructContextChildren(InChildNodes);
}

template<typename ContextMenuType>
inline FPhraseContextMenuNode<ContextMenuType>::FPhraseContextMenuNode(const TCHAR* InInputString, const float MenuScalar, TDelegate<TSharedPtr<IMenu>()> InOnGetMenu, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes)
	: FPhraseNode(InInputString, InOnPhraseParsed)
	, ContextMenuScalar(MenuScalar)
	, OnGetMenu(InOnGetMenu)
{
	static_assert(std::is_base_of<UPhraseTreeContextMenuObject, ContextMenuType>::value, "ContextType must be a subclass of UPhraseTreeContextMenuObject");

	ConstructContextChildren(InChildNodes);
}


template<typename ContextMenuType>
FORCEINLINE FPhraseContextMenuNode<ContextMenuType>::~FPhraseContextMenuNode()
{

}


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

UPhraseTreeContextObject* FPhraseContextMenuNode<UPhraseTreeContextMenuObject>::CreateContextObject()
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

	UPhraseTreeContextMenuObject* NewContextObject = NewObject<UPhraseTreeContextMenuObject>();
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
*/
