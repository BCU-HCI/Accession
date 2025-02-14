// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Utils/GraphQuadTree.h"


class FGraphQuadTree;

class FGraphQTNode : public TSharedFromThis<FGraphQTNode>
{
	friend FGraphQuadTree;

public:

	FGraphQTNode()
		:Owner(nullptr), TopLeft(FVector2D::ZeroVector), BotRight(FVector2D::ZeroVector),
		LocalTopLeft(FVector2D::ZeroVector), LocalBotRight(FVector2D::ZeroVector), Depth(0)
	{
		
	}

	FGraphQTNode(const FGraphQuadTree* Owner, int8 Depth = 0)
		: Owner(Owner), TopLeft(FVector2D::ZeroVector), BotRight(FVector2D::ZeroVector),
		LocalTopLeft(FVector2D::ZeroVector), LocalBotRight(FVector2D::ZeroVector), Depth(Depth)
	{

	}

	FGraphQTNode(
		const FGraphQuadTree* Owner, 
		FVector2D TopLeft, FVector2D BotRight, 
		FVector2D LocalTopLeft, FVector2D LocalBotRight, 
		int8 Depth = 0
	) : Owner(Owner), TopLeft(TopLeft), BotRight(BotRight), LocalTopLeft(LocalTopLeft), LocalBotRight(LocalBotRight), Depth(Depth)
	{
		
	}

	/**
	 * Checks if Subsegments Exist on this Node.
	 * @return True, if Child Segments were Found, otherwise False.
	 */
	bool ContainsSegments() const
	{
		return Children.Num() > 0;
	}

	/**
	 * Checks if Graph Nodes Exist on this Node, and descendants.
	 * @return True, if Graph Nodes were Found, otherwise False.
	 */
	bool ContainsGraphNodes() const
	{
		return ContainedNodes.Num() > 0;
	}


	/**
	 * Gets the Child Segments of this Node.
	 * @return Array of Child Segments.
	 */
	TArray<TSharedPtr<FGraphQTNode>> GetChildNodes() const
	{
		return Children;
	}


	/**
	 * Checks if a Graph Node is Contained in this Quad Tree Segment.
	 * @param NodeTopLeft Top-Left Graph Position of the Node.
	 * @param NodeBotRight Bottom-Right Graph Position of the Node.
 	 * @return 
	 */
	bool ContainsNodeRect(const FVector2D NodeTopLeft, const FVector2D NodeBotRight) const
	{
		return (NodeTopLeft.ComponentwiseAllGreaterOrEqual(TopLeft) && NodeTopLeft.ComponentwiseAllLessOrEqual(BotRight)) || 
			(NodeBotRight.ComponentwiseAllGreaterOrEqual(TopLeft) && NodeBotRight.ComponentwiseAllLessOrEqual(BotRight));
	}

	/**
	 * Partitions the Space of this Quad Tree Segment.
	 * @param MinSegmentSize Minimum Size of a Child Segment.
	 * @return Array of Child Segments, otherwise an Empty Array if SegmentSize is too small.
	 */
	TArray<TSharedPtr<FGraphQTNode>>& PartitionSpace(FVector2D MinSegmentSize = FVector2D(300, 250))
	{
		if (ContainsSegments())
			return Children;

		FVector2D SegmentSize = (BotRight - TopLeft) / 2;
		if (SegmentSize.ComponentwiseAllLessThan(MinSegmentSize)) // Ensure New Segments Won't Be Too Small.
			return Children;

		FVector2D LocalSegmentSize = (LocalBotRight - LocalTopLeft) / 2;

		// Size Array For Incoming Segments
		Children.Reserve(4);

		for (int Y = 0; Y < 2; ++Y)
		{
			for (int X = 0; X < 2; ++X)
			{
				FVector2D SegmentOffset(X * SegmentSize.X, Y * SegmentSize.Y);
				FVector2D LocalSegmentOffset(X * SegmentSize.X, Y * SegmentSize.Y);

				Children.Add(
					MakeShared<FGraphQTNode>(
						Owner,
						TopLeft + SegmentOffset,
						TopLeft + SegmentOffset + SegmentSize,
						LocalTopLeft + LocalSegmentOffset,
						LocalTopLeft + LocalSegmentOffset + LocalSegmentSize,
						Depth + 1
					)
				);
			}
		}

		return Children;
	}

	/**
	 * Visualizes the Quad Tree Segment, and Descendants.
	 * @param ElementList Windows Element List, to Visualize From.
	 * @param LinePoints Array of Line Positions, to Draw Using.
	 * @param PaintGeometry Paint Geometry of the Graph.
	 * @param LayerID LayerID of the Graph.
	 * @param LineColor Color of the Line, when Drawn.
	 */
	void Visualize(FSlateWindowElementList& ElementList, TArray<FVector2D>& LinePoints, const FPaintGeometry& PaintGeometry, const int32& LayerID, const FLinearColor& LineColor = FLinearColor::Green)
	{
		// Draw Children
		if (Children.IsEmpty())
			return;

		// Draw Vertical Line
		LinePoints[0] = LocalTopLeft + FVector2D((LocalBotRight.X - LocalTopLeft.X) / 2, 0);
		LinePoints[1] = LocalTopLeft + FVector2D((LocalBotRight.X - LocalTopLeft.X) / 2, LocalBotRight.Y - LocalTopLeft.Y);

		FSlateDrawElement::MakeLines(ElementList, LayerID, PaintGeometry, LinePoints, ESlateDrawEffect::None, LineColor);

		// Draw Horizontal Line
		LinePoints[0] = LocalTopLeft + FVector2D(0, (LocalBotRight.Y - LocalTopLeft.Y ) / 2);
		LinePoints[1] = LocalTopLeft + FVector2D(LocalBotRight.X - LocalTopLeft.X, (LocalBotRight.Y - LocalTopLeft.Y ) / 2);

		FSlateDrawElement::MakeLines(ElementList, LayerID, PaintGeometry, LinePoints, ESlateDrawEffect::None, LineColor);

		// Draw Children

		for (auto& Child : Children)
		{
			Child->Visualize(ElementList, LinePoints, PaintGeometry, LayerID, LineColor);
		}

	}

protected:

	/**
	 * Quad Tree Segment Owner.
	 */
	const FGraphQuadTree* Owner;


	/**
	 * Top Left Position of this Segment, in Graph Space.
	 */
	FVector2D TopLeft;

	/**
	 * Bottom Right Position of this Segment, in Graph Space.
	 */
	FVector2D BotRight;

	/**
	 * Top Left Position of this Segment, in Local Panel Space.
	 */
	FVector2D LocalTopLeft;

	/**
	 * Bottom Right Position of this Segment, in Local Panel Space.

	 */
	FVector2D LocalBotRight;

	/**
	 * Depth of this Node in the Quad Tree.
	 */
	int8 Depth;

	/**
	 * Child Segments from this Node.
	 */
	TArray<TSharedPtr<FGraphQTNode>> Children;

	/**
	 * GraphNodes Contained in this Segment.
	 */
	TArray<const UEdGraphNode*> ContainedNodes;
};
