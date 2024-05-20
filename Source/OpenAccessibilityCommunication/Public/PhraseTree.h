// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PhraseTree/PhraseNode.h"
#include "PhraseTree/Containers/ParseRecord.h"
#include "PhraseTree/Containers/ContextObject.h"

enum EPhraseTreeBranchBindResult : uint8_t
{
	/// <summary>
	/// test
	/// </summary>
	BRANCH_NOT_BOUND,

	/// <summary>
	/// test
	/// </summary>
	BRANCH_BOUND,
	BRANCH_SPLIT
};

struct OPENACCESSIBILITYCOMMUNICATION_API FPhraseTreeBranchBind
{
	FPhraseTreeBranchBind()
	{

	}

	FPhraseTreeBranchBind(TPhraseNode InRootNode, TPhraseNode InBranchRoot)
	{
		StartNode = InRootNode;
		BranchRoot = InBranchRoot;
	}

	~FPhraseTreeBranchBind()
	{
		StartNode.Reset();
		BranchRoot.Reset();
	}

	/// <summary>
	/// The Node to start the binding of this branch root.
	/// </summary>
	TPhraseNode StartNode;

	/// <summary>
	/// The Root Node of the Branch that needs to be bound.
	/// </summary>
	TPhraseNode BranchRoot;
};

struct OPENACCESSIBILITYCOMMUNICATION_API FPhraseTreeContextManager
{
public:

	FPhraseTreeContextManager()
	{

	}

	~FPhraseTreeContextManager()
	{

	}

	// Context Stack Management

	/// <summary>
	/// Is the Context Stack Empty.
	/// </summary>
	void IsEmpty()
	{
		this->ContextObjectStack.IsEmpty();
	}

	/// <summary>
	/// Does the Context Stack Contain Any Context Objects.
	/// </summary>
	/// <returns>True, if Context Objects are on the stack. Otherwise False.</returns>
	bool HasContextObjects()
	{
		return this->ContextObjectStack.Num() > 0;
	}

	/// <summary>
	/// Does the Context Stack Contain The Given Context Object.
	/// </summary>
	/// <param name="InContextObject">- The Context Object To Check if On The Stack.</param>
	/// <returns>True, if the Context Object is Contained on the Stack.</returns>
	bool HasContextObject(UPhraseTreeContextObject* InContextObject)
	{
		return this->ContextObjectStack.Contains(InContextObject);
	}

	/// <summary>
	/// Gets the Entire Context Stack.
	/// </summary>
	/// <returns>An Array Containing the Current Context Stack.</returns>
	TArray<UPhraseTreeContextObject*> GetContextStack()
	{
		return this->ContextObjectStack;
	}

	// Context Stack Ammendments

	/// <summary>
	/// Peeks the Top Context Object On The Stack.
	/// </summary>
	/// <param name="OutContextObject">- Returns the Top Context Object.</param>
	void PeekContextObject(UPhraseTreeContextObject* OutContextObject)
	{
		OutContextObject = this->ContextObjectStack.Top();
	}

	/// <summary>
	/// Peeks the Top Context Object On The Stack.
	/// </summary>
	/// <returns>The Top Context Object on the Stack.</returns>
	UPhraseTreeContextObject* PeekContextObject()
	{
		return this->ContextObjectStack.Top();
	}

	/// <summary>
	/// Pushes a Context Object onto the Stack.
	/// </summary>
	/// <param name="InContextObject">- The Context Object To Push Onto The Stack.</param>
	void PushContextObject(UPhraseTreeContextObject* InContextObject)
	{
		this->ContextObjectStack.Push(InContextObject);
	}
	
	/// <summary>
	/// Pops the Top Context Object From The Stack.
	/// </summary>
	void PopContextObject()
	{
		this->ContextObjectStack.Pop();
	}

	/// <summary>
	/// Pops the Top Context Object From The Stack.
	/// </summary>
	/// <typeparam name="CastToContextType">DownCast Type for the Popped Context Object. (Must be Derrived From UPhraseTreeContextObject).</typeparam>
	/// <param name="OutContextObject">- Returns the Popped Downcasted Context Object From the Stack.</param>
	template<class CastToContextType>
	void PopContextObject(CastToContextType* OutContextObject)
	{
		OutContextObject = Cast<CastToContextType>(this->ContextObjectStack.Pop());
	}

	/// <summary>
	/// Pops the Top Context Object From The Stack.
	/// </summary>
	/// <param name="OutContextObject">- Returns the Popped Context Object From the Stack.</param>
	void PopContextObject(UPhraseTreeContextObject* OutContextObject)
	{
		OutContextObject = this->ContextObjectStack.Pop();
	}

	/// <summary>
	/// Updates the Context Stack with the Given Array of Context Objects.
	/// </summary>
	/// <param name="InContextObjectStack"></param>
	void UpdateContextStack(TArray<UPhraseTreeContextObject*> InContextObjectStack)
	{
		this->ContextObjectStack = InContextObjectStack;

		FilterContextStack();
	}

	// Context Stack Filtering

	/// <summary>
	/// Filters the Context Stack, removing any Context Objects that are no longer active and any of its derived context objects.
	/// </summary>
	void FilterContextStack()
	{
		bool bRemoveDerivedContextObjects = false;

		int i = this->ContextObjectStack.Num() - 1;
		if (i < 0)
			return;

		UPhraseTreeContextObject* CurrObj = nullptr;

		do
		{
			CurrObj = this->ContextObjectStack[i];

			if (CurrObj != nullptr && CurrObj->GetIsActive())
			{
				i--;
				continue;
			}

			if (CurrObj->IsValidLowLevel())
			{
				CurrObj->RemoveFromRoot();
				CurrObj->MarkAsGarbage();
			}

			this->ContextObjectStack.RemoveAt(i);
			i--;

		} while (i > 0);

		CurrObj = nullptr;
	}

private:

	TArray<UPhraseTreeContextObject*> ContextObjectStack;

};

/**
 * 
 */
class OPENACCESSIBILITYCOMMUNICATION_API FPhraseTree : public FPhraseNode
{
public:
	FPhraseTree();
	~FPhraseTree();

	bool Tick(float DeltaTime);

	// FPhaseNode Implementation
	virtual FParseResult ParsePhrase(TArray<FString>& InPhraseWordArray, FParseRecord& InParseRecord) override;
	// End FPhaseNode Implementation

	/// <summary>
	/// Bind a branch to the tree.
	/// Attaching to any overlapping nodes.
	/// </summary>
	/// <param name="InNode">The constructed branch to attach to the tree.</param>
	void BindBranch(const TPhraseNode& InNode);
	
	/// <summary>
	/// Bind Multiple Branches to the Tree, that are not connected.
	/// </summary>
	void BindBranches(const TPhraseNodeArray& InNodes);

	/// <summary>
	/// Parses and Propogates the given Transcription Segments down the tree.
	/// </summary>
	/// <param name="InTranscriptionSegments"></param>
	void ParseTranscription(TArray<FString> InTranscriptionSegments);

private:

	/// <summary>
	/// The Node that was last visited in the tree.
	/// Allowing for quick entry, due to split phrases.
	/// </summary>
	TSharedPtr<FPhraseNode> LastVistedNode;

	/// <summary>
	/// The Parse Record that was achieved from the last visit to the tree that failed.
	/// Allowing for correct propogation of Record Dependant Nodes, with correct inputs.
	/// </summary>
	TUniquePtr<FParseRecord> LastVistedParseRecord;

	/// <summary>
	/// The Manager for the Context Objects, that are currently active in the Tree.
	/// With them being stored in a stack context.
	/// </summary>
	FPhraseTreeContextManager ContextManager;

	FTSTicker::FDelegateHandle TickDelegateHandle;
};
