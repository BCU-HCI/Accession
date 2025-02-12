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

	bool ContainsSegments() const
	{
		return Children.Num() > 0;
	}

	bool ContainsGraphNodes() const
	{
		return ContainedNodes.Num() > 0;
	}


	TArray<TSharedPtr<FGraphQTNode>> GetChildNodes() const
	{
		return Children;
	}


	bool ContainsNodeRect(const FVector2D NodeTopLeft, const FVector2D NodeBotRight) const
	{
		return (NodeTopLeft.ComponentwiseAllGreaterOrEqual(TopLeft) && NodeTopLeft.ComponentwiseAllLessOrEqual(BotRight)) || 
			(NodeBotRight.ComponentwiseAllGreaterOrEqual(TopLeft) && NodeBotRight.ComponentwiseAllLessOrEqual(BotRight));
	}

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

	const FGraphQuadTree* Owner;

	FVector2D TopLeft;
	FVector2D BotRight;

	FVector2D LocalTopLeft;
	FVector2D LocalBotRight;

	int8 Depth;

	TArray<TSharedPtr<FGraphQTNode>> Children;
	TArray<const UEdGraphNode*> ContainedNodes;
};
