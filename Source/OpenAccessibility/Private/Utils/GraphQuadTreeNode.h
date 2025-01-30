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

protected:

	TWeakPtr<GraphQuadTree> Owner;

	FVector2D TopLeft;
	FVector2D BotRight;

	int8 Depth;

	TArray<TSharedPtr<GraphQTNode>> Children;
	TArray<const UEdGraphNode*> ContainedNodes;
};