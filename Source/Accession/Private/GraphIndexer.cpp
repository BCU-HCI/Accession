// Copyright (C) HCI-BCU 2025. All rights reserved.

#include "GraphIndexer.h"

#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "GraphEditAction.h"
#include "AccessionLogging.h"

FGraphIndexer::FGraphIndexer()
{
}

FGraphIndexer::FGraphIndexer(const UEdGraph *GraphToIndex)
	: LinkedGraph(const_cast<UEdGraph *>(GraphToIndex))
{
	BuildGraphIndex();

	OnGraphChangedHandle = LinkedGraph->AddOnGraphChangedHandler(
		FOnGraphChanged::FDelegate::CreateRaw(this, &FGraphIndexer::OnGraphEvent));
}

FGraphIndexer::~FGraphIndexer()
{
	IndexMap.Empty();
	NodeSet.Empty();
	AvailableIndices.Empty();

	LinkedGraph->RemoveOnGraphChangedHandler(OnGraphChangedHandle);

	LinkedGraph = nullptr;
}

bool FGraphIndexer::ContainsKey(const int &InKey)
{
	return IndexMap.Contains(InKey);
}

int FGraphIndexer::ContainsNode(UEdGraphNode *InNode)
{
	check(InNode != nullptr);

	if (!InNode->IsValidLowLevelFast() || !NodeSet.Contains(InNode->GetUniqueID()))
		return -1;

	const int *ReturnedIndex = IndexMap.FindKey(InNode);

	if (ReturnedIndex != nullptr)
	{
		return *ReturnedIndex;
	}
	else
		return -1;
}

void FGraphIndexer::ContainsNode(UEdGraphNode *InNode, int &OutIndex)
{
	OutIndex = ContainsNode(InNode);
}

int FGraphIndexer::GetKey(const UEdGraphNode *InNode)
{
	check(InNode != nullptr);

	if (!InNode->IsValidLowLevelFast())
		return -1;

	const int *FoundKey = IndexMap.FindKey(const_cast<UEdGraphNode *>(InNode));

	if (FoundKey != nullptr)
		return *FoundKey;
	else
		return -1;
}

bool FGraphIndexer::GetKey(const UEdGraphNode *InNode, int &OutKey)
{
	check(InNode != nullptr);

	if (!InNode->IsValidLowLevelFast())
		return false;

	const int *FoundKey = IndexMap.FindKey(const_cast<UEdGraphNode *>(InNode));
	if (FoundKey != nullptr)
	{
		OutKey = *FoundKey;
		return true;
	}
	else
		return false;
}

UEdGraphNode *FGraphIndexer::GetNode(const int &InIndex)
{
	if (!IndexMap.Contains(InIndex))
	{
		UE_LOG(LogAccession, Warning, TEXT("Provided Index is not recognised"))

		return nullptr;
	}

	return IndexMap[InIndex];
}

void FGraphIndexer::GetPin(const int &InNodeIndex, const int &InPinIndex, UEdGraphPin *OutPin)
{
	UEdGraphNode *Node = GetNode(InNodeIndex);
	if (Node == nullptr)
	{
		UE_LOG(LogAccession, Warning, TEXT("Requested Node at index %d is not valid."), InNodeIndex);
		return;
	}

	OutPin = Node->GetPinAt(InPinIndex); // Returns nullptr if invalid
}

UEdGraphPin *FGraphIndexer::GetPin(const int &InNodeIndex, const int &InPinIndex)
{
	UEdGraphNode *Node = GetNode(InNodeIndex);
	if (Node == nullptr)
	{
		UE_LOG(LogAccession, Warning, TEXT("Requested Node at index %d is not valid."), InNodeIndex);
		return nullptr;
	}

	return Node->GetPinAt(InPinIndex); // Returns nullptr if invalid
}

void FGraphIndexer::GetNode(const int &InIndex, UEdGraphNode *OutNode)
{
	OutNode = GetNode(InIndex);
}

int FGraphIndexer::AddNode(const UEdGraphNode *InNode)
{
	check(InNode != nullptr);

	if (!InNode->IsValidLowLevelFast())
	{
		UE_LOG(LogAccession, Warning, TEXT("Provided Node is not valid."))
	}

	int Index = ContainsNode(const_cast<UEdGraphNode *>(InNode));
	if (Index != -1)
	{
		return Index;
	}

	GetAvailableIndex(Index);

	NodeSet.Add(InNode->GetUniqueID());
	IndexMap.Add(Index, const_cast<UEdGraphNode *>(InNode));

	return Index;
}

void FGraphIndexer::AddNode(int &OutIndex, const UEdGraphNode &InNode)
{
	OutIndex = AddNode(&InNode);
}

int FGraphIndexer::GetOrAddNode(const UEdGraphNode *InNode)
{
	int Key = GetKey(InNode);
	if (Key != -1)
	{
		return Key;
	}

	return AddNode(InNode);
}

void FGraphIndexer::GetOrAddNode(const UEdGraphNode *InNode, int &OutIndex)
{
	OutIndex = GetKey(InNode);
	if (OutIndex != -1)
	{
		return;
	}

	OutIndex = AddNode(InNode);
}

void FGraphIndexer::RemoveNode(const int &InIndex)
{
	if (!IndexMap.Contains(InIndex))
	{
		UE_LOG(LogAccession, Warning, TEXT("Provided Index is not recognised"))
	}

	const UEdGraphNode *Node = IndexMap[InIndex];

	if (Node->IsValidLowLevelFast())
	{
		NodeSet.Remove(Node->GetUniqueID());
		IndexMap.Remove(InIndex);
		AddAvailableIndex(InIndex);
	}
	else
	{
		UE_LOG(LogAccession, Warning, TEXT("Stored Node in IndexMap is not vaild."))
	}
}

void FGraphIndexer::RemoveNode(const UEdGraphNode *InNode)
{
	check(InNode != nullptr);

	int Key = GetKey(InNode);
	if (Key == -1)
	{
		UE_LOG(LogAccession, Warning, TEXT("Node does not exist in IndexMap."))
		return;
	}

	RemoveNode(Key);
}

// ------------------------
//	Graph Events
// ------------------------

void FGraphIndexer::OnGraphEvent(const FEdGraphEditAction &InAction)
{
	if (InAction.Graph != LinkedGraph)
	{
		return;
	}

	switch (InAction.Action)
	{
	case EEdGraphActionType::GRAPHACTION_AddNode:
	{
		for (const UEdGraphNode *Node : InAction.Nodes)
		{
			AddNode(Node);
		}

		break;
	}

	case EEdGraphActionType::GRAPHACTION_RemoveNode:
	{
		for (const UEdGraphNode *Node : InAction.Nodes)
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
	AvailableIndices.Empty();

	BuildGraphIndex();
}

void FGraphIndexer::AddAvailableIndex(const int32 &NewIndex)
{
	const int32 FoundIndex = AvailableIndices.IndexOfByPredicate([NewIndex](const int32 &Element)
																 { return Element < NewIndex; });

	if (FoundIndex == INDEX_NONE)
	{
		AvailableIndices.Add(NewIndex);
	}
	else
	{
		AvailableIndices.Insert(NewIndex, FoundIndex);
	}

	UE_LOG(LogTemp, Log, TEXT("Added Index %d to AvailableIndices"), NewIndex);
}

int FGraphIndexer::GetAvailableIndex()
{
	if (!AvailableIndices.IsEmpty())
	{
		return AvailableIndices.Pop();
	}

	return IndexMap.Num();
}

void FGraphIndexer::GetAvailableIndex(int &OutIndex)
{
	if (!AvailableIndices.IsEmpty())
	{
		OutIndex = AvailableIndices.Pop();
	}
	else
		OutIndex = IndexMap.Num();
}

void FGraphIndexer::BuildGraphIndex()
{
	if (LinkedGraph == nullptr)
		return;

	for (TObjectPtr<UEdGraphNode> Node : LinkedGraph->Nodes)
	{
		AddNode(Node);
	}
}
