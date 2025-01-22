// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GraphQuadTree.h"

class GraphQuadTree;

class GraphQTNode : public TSharedFromThis<GraphQTNode>
{
	friend GraphQuadTree;

public:

	GraphQTNode(TSharedRef<GraphQuadTree> Owner)
		: Owner(Owner), TopLeft(FVector2D::ZeroVector), BotRight(FVector2D::ZeroVector)
	{

	}

	GraphQTNode(TSharedRef<GraphQuadTree> Owner, FVector2D TopLeft, FVector2D BotRight)
		: Owner(Owner), TopLeft(TopLeft), BotRight(BotRight)
	{
		
	}

	bool AddGraphNode(UEdGraphNode* GraphNode)
	{
		FVector2D GNTopLeft = FVector2D(GraphNode->NodePosX, GraphNode->NodePosY);
		FVector2D GNBotRight = FVector2D(GraphNode->NodePosX + GraphNode->NodeWidth, GraphNode->NodePosY + GraphNode->NodeHeight);

		// Contained in Viewport.
		if (GNTopLeft.ComponentwiseAllGreaterOrEqual(this->TopLeft) 
			&& GNBotRight.ComponentwiseAllLessOrEqual(this->BotRight))
		{
			ContainedNodes.Add(GraphNode);

			return true;
		}

		// Out of Viewport
		return false;
	}

	TArray<TSharedPtr<GraphQTNode>> GetChildNodes()
	{
		return Children;
	}

protected:

	TWeakPtr<GraphQuadTree> Owner;

	FVector2D TopLeft;
	FVector2D BotRight;

	TArray<TSharedPtr<GraphQTNode>> Children;
	TArray<UEdGraphNode*> ContainedNodes;
};