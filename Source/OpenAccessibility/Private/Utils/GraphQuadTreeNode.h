// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GraphQuadTree.h"
#include "Rendering/SlateDrawBuffer.h"

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

	void Visualize(FSlateWindowElementList& ElementList, const FPaintGeometry& PaintGeometry, const int32& LayerID, const FLinearColor& LineColor = FLinearColor::Green)
	{
		{
			TArray<FVector2D> Lines;
			Lines.Reserve(2);

			// Draw Horizontal Line
			Lines[0] = FVector2D(
				TopLeft.X, 
				TopLeft.Y + ((BotRight.Y - TopLeft.Y) / 2)
			);

			Lines[1] = FVector2D(
				BotRight.X,
				TopLeft.Y + ((BotRight.Y - TopLeft.Y) / 2)
			);

			FSlateDrawElement::MakeLines(
				ElementList,
				LayerID,
				PaintGeometry,
				Lines
			);


			// Draw Vertical Line
			Lines[0] = FVector2D(
				TopLeft.X + ((BotRight.X - TopLeft.X) / 2),
				TopLeft.Y
			);

			Lines[1] = FVector2D(
				TopLeft.X + ((BotRight.X - TopLeft.X) / 2),
				BotRight.Y
			);

			FSlateDrawElement::MakeLines(
				ElementList,
				LayerID,
				PaintGeometry,
				Lines
			);
		}

		// Draw Children
		if (Children.IsEmpty())
			return;

		for (auto& Child : Children)
		{
			Child->Visualize(ElementList, PaintGeometry, LayerID, LineColor);
		}

	}

protected:

	TWeakPtr<GraphQuadTree> Owner;

	FVector2D TopLeft;
	FVector2D BotRight;

	int8 Depth;

	TArray<TSharedPtr<GraphQTNode>> Children;
	TArray<const UEdGraphNode*> ContainedNodes;
};