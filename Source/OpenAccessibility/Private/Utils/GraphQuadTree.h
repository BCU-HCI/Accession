// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "GraphEditor.h"
#include "SGraphPanel.h"

class GraphQuadTree
{
public:

	GraphQuadTree(UEdGraph* Graph)
	{
		
	}

	GraphQuadTree(SGraphEditor* GraphEditor)
	{

	}

private:

	UEdGraph* LinkedGraph;
	SGraphEditor* LinkedEditor;
};

class GraphQTNode
{
public:

	GraphQTNode(GraphQuadTree* Owner, FVector2D TopLeft, FVector2D BotRight, TArray<GraphQTNode*> Nodes)
	{
		
	}

	void AddGraphNode(UEdGraphNode*)

	TArray<GraphQTNode> GetChildNodes()
	{
		return Children;
	}

private:


private:

	FVector2D TopLeft;
	FVector2D BotRight;

	TArray<GraphQTNode*> Children;
	TArray<UEdGraphNode*> ContainedNodes;
	
	const GraphQuadTree* Owner;
};