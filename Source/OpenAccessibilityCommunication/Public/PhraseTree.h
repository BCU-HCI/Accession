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

	void IsEmpty()
	{
		this->ContextObjectStack.IsEmpty();
	}

	bool HasContextObjects()
	{
		return this->ContextObjectStack.Num() > 0;
	}

	bool HasContextObject(UPhraseTreeContextObject* InContextObject)
	{
		return this->ContextObjectStack.Contains(InContextObject);
	}

	TArray<UPhraseTreeContextObject*> GetContextStack()
	{
		return this->ContextObjectStack;
	}

	// Context Stack Ammendments

	void PeekContextObject(UPhraseTreeContextObject* OutContextObject)
	{
		OutContextObject = this->ContextObjectStack.Top();
	}

	UPhraseTreeContextObject* PeekContextObject()
	{
		return this->ContextObjectStack.Top();
	}

	void PushContextObject(UPhraseTreeContextObject* InContextObject)
	{
		this->ContextObjectStack.Push(InContextObject);
	}
	
	void PopContextObject()
	{
		this->ContextObjectStack.Pop();
	}

	template<class CastToContextType>
	void PopContextObject(CastToContextType* OutContextObject)
	{
		OutContextObject = Cast<CastToContextType>(this->ContextObjectStack.Pop());
	}

	void PopContextObject(UPhraseTreeContextObject* OutContextObject)
	{
		OutContextObject = this->ContextObjectStack.Pop();
	}

	void UpdateContextStack(TArray<UPhraseTreeContextObject*> InContextObjectStack)
	{
		this->ContextObjectStack = InContextObjectStack;

		FilterContextStack();
	}

	// Context Stack Filtering

	void FilterContextStack()
	{
		bool bRemoveDerivedContextObjects = false;

		int i = 0;
		while (i < this->ContextObjectStack.Num())
		{
			if (this->ContextObjectStack[i]->IsValidLowLevel() && !bRemoveDerivedContextObjects)
				continue;

			bRemoveDerivedContextObjects = true;

			this->ContextObjectStack[i]->RemoveFromRoot();

			this->ContextObjectStack.RemoveAt(i);
			i--;

			if (i < 0)
				i = 0;
		}
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
	/// The Manager for the Context Objects, that are currently active in the Tree.
	/// With them being stored in a stack context.
	/// </summary>
	FPhraseTreeContextManager ContextManager;
};
