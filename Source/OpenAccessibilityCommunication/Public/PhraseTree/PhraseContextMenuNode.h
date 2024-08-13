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

	static_assert(std::is_base_of_v<UPhraseTreeContextMenuObject, ContextMenuType>, "ContextType must be a subclass of UPhraseTreeContextMenuObject");

	FPhraseContextMenuNode(const TCHAR* InInputString)
		: FPhraseNode(InInputString)
		, ContextMenuScalar(1.0f)
	{
		this->ChildNodes = TPhraseNodeArray();
	};

	FPhraseContextMenuNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes)
		: FPhraseNode(InInputString)
		, ContextMenuScalar(1.0f)
	{
		ConstructContextChildren(InChildNodes);
	};

	FPhraseContextMenuNode(const TCHAR* InInputString, TDelegate<TSharedPtr<IMenu>(FParseRecord& Record)> InOnGetMenu, TPhraseNodeArray InChildNodes)
		: FPhraseNode(InInputString)
		, ContextMenuScalar(1.0f)
		, OnGetMenu(InOnGetMenu)
	{
		ConstructContextChildren(InChildNodes);
	};

	FPhraseContextMenuNode(const TCHAR* InInputString, const float InMenuScalar, TPhraseNodeArray InChildNodes)
		: FPhraseNode(InInputString)
		, ContextMenuScalar(InMenuScalar)
	{
		ConstructContextChildren(InChildNodes);
	};

	FPhraseContextMenuNode(const TCHAR* InInputString, const float InMenuScalar, TDelegate<TSharedPtr<IMenu>(FParseRecord& Record)> InOnGetMenu, TPhraseNodeArray InChildNodes)
		: FPhraseNode(InInputString)
		, ContextMenuScalar(InMenuScalar)
		, OnGetMenu(InOnGetMenu)
	{
		ConstructContextChildren(InChildNodes);
	}

	FPhraseContextMenuNode(const TCHAR* InInputString, const float InMenuScalar, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes)
		: FPhraseNode(InInputString, InOnPhraseParsed)
		, ContextMenuScalar(InMenuScalar)
	{
		ConstructContextChildren(InChildNodes);
	}

	FPhraseContextMenuNode(const TCHAR* InInputString, const float InMenuScalar, TDelegate<TSharedPtr<IMenu>(FParseRecord& Record)> InOnGetMenu, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes)
		: FPhraseNode(InInputString, InOnPhraseParsed)
		, ContextMenuScalar(InMenuScalar)
		, OnGetMenu(InOnGetMenu)
	{
		ConstructContextChildren(InChildNodes);
	}

	virtual ~FPhraseContextMenuNode()
	{

	}

	// FPhraseNode Implementation

	/// <summary>
	/// Parses the phrase down the given Node, propagating down child nodes if required.
	/// </summary>
	/// <param name="InPhraseWordArray">The Array of Phrase Strings to Propogate against.</param>
	/// <param name="InParseRecord">The Record of Propagation of collected context's and inputs.</param>
	/// <returns>Returns the Result of the propogation, including any key nodes met.</returns>
	virtual FParseResult ParsePhrase(TArray<FString>& InPhraseWordArray, FParseRecord& InParseRecord) override;

	/// <summary>
	/// Parses the phrase down the given node, propagating down child nodes if required.
	/// Missed Pop of the Phrase Array from this Node.
	/// </summary>
	/// <param name="InPhraseWordArray"></param>
	/// <param name="InParseRecord"></param>
	/// <returns>Returns the Result of the propogation, including any key nodes met.</returns>
	virtual FParseResult ParsePhraseAsContext(TArray<FString>& InPhraseWordArray, FParseRecord& InParseRecord) override;

	// End FPhraseNode Implementation

protected:

	// FPhraseContextNodeBase Implementation

	/// <summary>
	/// Checks if the Given Context Array Contains Context Objects.
	/// </summary>
	/// <param name="InContextObjects">- The Array To Check For Context Objects.</param>
	/// <returns>True, if their is Context Objects in the Given Array.</returns>
	virtual bool HasContextObject(TArray<UPhraseTreeContextObject*> InContextObjects) const override;

	/// <summary>
	/// Creates a Context Object, using Record Inputs.
	/// </summary>
	/// <returns>The Created Context Object, otherwise nullptr</returns>
	virtual UPhraseTreeContextObject* CreateContextObject(FParseRecord& Record) override;

	/// <summary>
	/// Constructs the Context Nodes Children, from Given Child Nodes.
	/// Allowing for Inclusion of Utility Nodes in relation to the Context.
	/// </summary>
	/// <param name="InChildNodes">- An Array of the Nodes Children.</param>
	virtual void ConstructContextChildren(TPhraseNodeArray& InChildNodes) override;

	// End FPhraseContextNode Implementation

protected:

	/// <summary>
	///	Scalar for the Initialized Menu Elements.
	/// </summary>
	const float ContextMenuScalar;

	/// <summary>
	///	Delegate for Initializing of the Menu.
	/// </summary>
	TDelegate<TSharedPtr<IMenu>(FParseRecord& Record)> OnGetMenu;
};

template<typename ContextMenuType>
FParseResult FPhraseContextMenuNode<ContextMenuType>::ParsePhrase(TArray<FString>& InPhraseWordArray, FParseRecord& InParseRecord)
{
	if (!HasContextObject(InParseRecord.GetContextStack()))
	{
		UPhraseTreeContextObject* NewObject = CreateContextObject(InParseRecord);

		InParseRecord.PushContextObj(NewObject);
	}

	if (InPhraseWordArray.IsEmpty())
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Emptied Phrase Array ||"))

		return FParseResult(PHRASE_REQUIRES_MORE, AsShared());
	}

	InPhraseWordArray.Pop();

	OnPhraseParsed.ExecuteIfBound(InParseRecord);

	return ParseChildren(InPhraseWordArray, InParseRecord);

	return FPhraseNode::ParsePhrase(InPhraseWordArray, InParseRecord);
}

template<typename ContextMenuType>
inline FParseResult FPhraseContextMenuNode<ContextMenuType>::ParsePhraseAsContext(TArray<FString>& InPhraseWordArray, FParseRecord& InParseRecord)
{
	if (!HasContextObject(InParseRecord.GetContextStack()))
	{
		UPhraseTreeContextObject* NewObject = CreateContextObject(InParseRecord);

		InParseRecord.PushContextObj(NewObject);
	}

	if (InPhraseWordArray.IsEmpty())
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Emptied Phrase Array ||"))

		return FParseResult(PHRASE_REQUIRES_MORE, AsShared());
	}

	OnPhraseParsed.ExecuteIfBound(InParseRecord);

	return ParseChildren(InPhraseWordArray, InParseRecord);
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
UPhraseTreeContextObject* FPhraseContextMenuNode<ContextMenuType>::CreateContextObject(FParseRecord& Record)
{
	if (!OnGetMenu.IsBound())
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("OnGetMenu Delegate Not Bound. Cannot Create Context Object, linked to a Menu."));
		return nullptr;
	}

	TSharedPtr<IMenu> NewMenu = OnGetMenu.Execute(Record);

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