// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GraphQuadTree.h"

class GraphQuadTree;

class GraphQTNode : public TSharedFromThis<GraphQTNode>
{
	friend GraphQuadTree;

public:

	GraphQTNode(TSharedRef<GraphQuadTree> Owner, int8 Depth = 0)
		: Owner(Owner), TopLeft(FVector2D::ZeroVector), BotRight(FVector2D::ZeroVector), Depth(Depth)
	{

	}

	GraphQTNode(TSharedRef<GraphQuadTree> Owner, FVector2D TopLeft, FVector2D BotRight, int8 Depth = 0)
		: Owner(Owner), TopLeft(TopLeft), BotRight(BotRight), Depth(Depth)
	{
		
	}

	bool ContainsSegments() const
	{
		return Children.Num() > 0;
	}

	bool ContainsGraphNodes() const
	{
		return ContainedNodes.Num() > 0;
	}


	TArray<TSharedPtr<GraphQTNode>> GetChildNodes() const
	{
		return Children;
	}


	bool ContainsNodeRect(const FVector2D NodeTopLeft, const FVector2D NodeBotRight) const
	{
		bool ContainsTopLeft = NodeTopLeft.ComponentwiseAllGreaterOrEqual(TopLeft) &&
			NodeTopLeft.ComponentwiseAllLessOrEqual(TopLeft);

		bool ContainsBotRight = NodeBotRight.ComponentwiseAllGreaterOrEqual(BotRight) &&
			NodeBotRight.ComponentwiseAllLessOrEqual(BotRight);

		return ContainsTopLeft || ContainsBotRight;
	}

	/*
	bool AddGraphNode(const UEdGraphNode* GraphNode)
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
	*/

protected:

	TWeakPtr<GraphQuadTree> Owner;

	FVector2D TopLeft;
	FVector2D BotRight;
	int8 Depth;

	TArray<TSharedPtr<GraphQTNode>> Children;
	TArray<const UEdGraphNode*> ContainedNodes;
};