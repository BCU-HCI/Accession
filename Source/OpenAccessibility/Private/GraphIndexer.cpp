// Copyright F-Dudley. All Rights Reserved.


#include "GraphIndexer.h"

#include "GraphEditAction.h"
#include "OpenAccessibilityLogging.h"

FGraphIndexer::FGraphIndexer(UEdGraph* GraphToIndex)
{
	check(GraphToIndex != nullptr);

    if (!GraphToIndex->IsValidLowLevelFast())
    {
        UE_LOG(LogOpenAccessibilityWarning, Warning, TEXT("Provided Graph To Index is not valid."))
    }

	LinkedGraph = GraphToIndex;

	BuildGraphIndex();

	OnGraphChangedHandle = LinkedGraph->AddOnGraphChangedHandler(FOnGraphChanged::FDelegate::CreateRaw(this, &FGraphIndexer::OnGraphEvent));
}

FGraphIndexer::~FGraphIndexer()
{
	IndexMap.Empty();
	NodeSet.Empty();
	AvailableIndices.Empty();

	LinkedGraph->RemoveOnGraphChangedHandler(OnGraphChangedHandle);

	LinkedGraph = nullptr;
}

bool FGraphIndexer::ContainsKey(const int& InKey)
{
	return IndexMap.Contains(InKey);
}

int FGraphIndexer::ContainsNode(UEdGraphNode* InNode)
{
	check(InNode != nullptr);

	if (!InNode->IsValidLowLevelFast() || !NodeSet.Contains(InNode->GetUniqueID()))
		return -1;

    const int* ReturnedIndex = IndexMap.FindKey(InNode);

    if (ReturnedIndex != nullptr)
	{
		return *ReturnedIndex;
	}
    else return -1;
}

void FGraphIndexer::ContainsNode(UEdGraphNode* InNode, int& OutIndex)
{
	OutIndex = ContainsNode(InNode);
}

UEdGraphNode* FGraphIndexer::GetNode(const int& InIndex)
{
	if (!IndexMap.Contains(InIndex))
	{
		UE_LOG(LogOpenAccessibilityWarning, Warning, TEXT("Provided Index is not recognised"))

		return nullptr;
	}

	return IndexMap[InIndex];
}

void FGraphIndexer::GetNode(const int& InIndex, UEdGraphNode* OutNode)
{
	OutNode = GetNode(InIndex);
}

int FGraphIndexer::AddNode(const UEdGraphNode* InNode)
{
	check (InNode != nullptr);
	
	if (!InNode->IsValidLowLevelFast())
	{
		UE_LOG(LogOpenAccessibilityWarning, Warning, TEXT("Provided Node is not valid."))
	}

	int Index = ContainsNode(const_cast<UEdGraphNode*>(InNode));
	if (Index != -1)
	{
		return Index;
	}

	Index = GetAvailableIndex();

	NodeSet.Add(InNode->GetUniqueID());
	IndexMap.Add(Index, const_cast<UEdGraphNode*>(InNode));

	return Index;
}

void FGraphIndexer::AddNode(int& OutIndex, const UEdGraphNode& InNode)
{
	OutIndex = AddNode(&InNode);
}

void FGraphIndexer::RemoveNode(const int& InIndex)
{
	if (!IndexMap.Contains(InIndex))
	{
		UE_LOG(LogOpenAccessibilityWarning, Warning, TEXT("Provided Index is not recognised"))
	}
	
	const UEdGraphNode* Node = IndexMap[InIndex];

	if (Node->IsValidLowLevelFast())
	{
		NodeSet.Remove(Node->GetUniqueID());
		IndexMap.Remove(InIndex);
		AvailableIndices.Add(InIndex);
	}
	else
	{
		UE_LOG(LogOpenAccessibilityWarning, Warning, TEXT("Stored Node in IndexMap is not vaild."))
	}
}

void FGraphIndexer::RemoveNode(const UEdGraphNode* InNode)
{
	check(InNode != nullptr);

	const int* FoundIndex = IndexMap.FindKey(const_cast<UEdGraphNode*>(InNode));
	if (FoundIndex == nullptr)
	{
		UE_LOG(LogOpenAccessibilityWarning, Warning, TEXT("Node does not exist in IndexMap."))
		return;
	}

	RemoveNode(*FoundIndex);
}



// ------------------------
//	Graph Events
// ------------------------

void FGraphIndexer::OnGraphEvent(const FEdGraphEditAction& InAction)
{
	if (InAction.Graph != LinkedGraph)
	{
		return;
	}

	switch (InAction.Action)
	{
		case EEdGraphActionType::GRAPHACTION_AddNode:
		{
			for (const UEdGraphNode* Node : InAction.Nodes)
			{
				AddNode(Node);
			}

			break;
		}

		case EEdGraphActionType::GRAPHACTION_RemoveNode:
		{
			for (const UEdGraphNode* Node : InAction.Nodes)
			{
				RemoveNode(Node);
			}

			break;
		}
	}
}

void FGraphIndexer::OnGraphRebuild()
{
	IndexMap.Reset();
	NodeSet.Reset();
	AvailableIndices.Reset();

	BuildGraphIndex();
}

int FGraphIndexer::GetAvailableIndex()
{
	if (AvailableIndices.Num() > 0)
		return AvailableIndices.Pop();
	else
		return IndexMap.Num();
}

void FGraphIndexer::GetAvailableIndex(int& OutIndex)
{
	OutIndex = GetAvailableIndex();
}

void FGraphIndexer::BuildGraphIndex()
{
	for (TObjectPtr<UEdGraphNode> Node : LinkedGraph->Nodes)
	{
		AddNode(Node);
	}
}
