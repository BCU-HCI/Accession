// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class UEdGraph;
class UEdGraphNode;
struct FEdGraphEditAction;

/**
 * 
 */
class OPENACCESSIBILITY_API FGraphIndexer 
{
public:
	FGraphIndexer(const UEdGraph* GraphToIndex);
	~FGraphIndexer();

	bool ContainsKey(const int& InKey);

	int ContainsNode(UEdGraphNode* InNode);
	void ContainsNode(UEdGraphNode* InNode, int& OutIndex);

	void GetNode(const int& InIndex, UEdGraphNode* OutNode);
	UEdGraphNode* GetNode(const int& InIndex);

	int AddNode(const UEdGraphNode* Node);
	void AddNode(int& OutIndex, const UEdGraphNode& InNode);

	void RemoveNode(const int& InIndex);
	void RemoveNode(const UEdGraphNode* InNode);

	void OnGraphEvent(const FEdGraphEditAction& InAction);
	void OnGraphRebuild();

private:

	int GetAvailableIndex();
	void GetAvailableIndex(int& OutIndex);

	void BuildGraphIndex();

protected:
	TMap<int, UEdGraphNode*> IndexMap;
	TSet<int32> NodeSet;

	TArray<int32> AvailableIndices;

	UEdGraph* LinkedGraph;
	FDelegateHandle OnGraphChangedHandle;
};
