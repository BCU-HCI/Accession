// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "GraphEditor.h"
#include "SGraphPanel.h"

#include "GraphQuadTreeNode.h"

class GraphQuadTree : public TSharedFromThis<GraphQuadTree>
{
public:

#define ZERO_BIT_OFFSET(x) 0 << x
	enum class EQueryBias : uint8
	{
		NONE = ZERO_BIT_OFFSET(0),
		LEFT = ZERO_BIT_OFFSET(1),
		RIGHT = ZERO_BIT_OFFSET(2),
		UP = ZERO_BIT_OFFSET(3),
		DOWN = ZERO_BIT_OFFSET(4),
	};
#undef 

	GraphQuadTree(UEdGraph* Graph, int8 DepthLimit = 6, FVector2D MinSegmentSizeLimit = FVector2D())
	: LinkedGraph(Graph), LinkedEditor(nullptr), DepthLimit(DepthLimit), MinSegmentSize(MinSegmentSizeLimit)
	{
		TSharedPtr<SGraphEditor> GraphEditor = SGraphEditor::FindGraphEditorForGraph(Graph);
		LinkedEditor = GraphEditor;

		SGraphPanel* GraphPanel = GraphEditor->GetGraphPanel();

		FVector2D RootTopLeft = GraphPanel->GetViewOffset();
		FVector2D RootBotRight = RootTopLeft + GraphPanel->GetCachedGeometry().GetLocalSize();

		RootNode = MakeShared<GraphQTNode>(TSharedRef<GraphQuadTree>(this), RootTopLeft, RootBotRight);
	}

	GraphQuadTree(const TSharedPtr<SGraphEditor>& GraphEditor, int8 DepthLimit = 6, FVector2D MinSegmentSizeLimit = FVector2D())
	: LinkedGraph(GraphEditor->GetCurrentGraph()), LinkedEditor(GraphEditor), DepthLimit(DepthLimit), MinSegmentSize(MinSegmentSizeLimit)
	{
		SGraphPanel* GraphPanel = GraphEditor->GetGraphPanel();

		FVector2D RootTopLeft = GraphPanel->GetViewOffset();
		FVector2D RootBotRight = RootTopLeft + GraphPanel->GetCachedGeometry().GetLocalSize();

		RootNode = MakeShared<GraphQTNode>(TSharedRef<GraphQuadTree>(this), RootTopLeft, RootBotRight);
	}

	void AddGraphNode(const UEdGraphNode* GraphNode)
	{
		if (!RootNode.IsValid() || GraphNode != nullptr)
			return;

		FVector2D GNTopLeft = FVector2D(GraphNode->NodePosX, GraphNode->NodePosY);
		FVector2D GNBotRight = FVector2D(GraphNode->NodePosX + GraphNode->NodeWidth, GraphNode->NodePosY + GraphNode->NodeHeight);


		TSet<TSharedRef<GraphQTNode>> CheckedNodes;
		TQueue<TSharedRef<GraphQTNode>> NodesToCheck;
		NodesToCheck.Enqueue(RootNode.ToSharedRef());

		TSharedRef<GraphQTNode> QNode;

		while (!NodesToCheck.IsEmpty())
		{
			if (!NodesToCheck.Dequeue(QNode) || !QNode->ContainsNodeRect(GNTopLeft, GNBotRight))
				continue;

			QNode->ContainedNodes.Add(GraphNode);

			if (QNode->Depth <= DepthLimit && GetSegmentSize(QNode) > MinSegmentSize)
			{
				TArray<TSharedPtr<GraphQTNode>> ChildNodes;
				if (QNode->ContainsSegments())
					ChildNodes = QNode->Children;
				else
					ChildNodes = SegmentQNodeSpace(QNode);

				for (auto GraphQtNode : ChildNodes)
				{
					NodesToCheck.Enqueue(GraphQtNode.ToSharedRef());
				}				
			}
		}

		// Recursive Check For What QNodes The GraphNode TopLeft and BotRight ar Contained In.
	}

	FVector2D FindOptimalLocation(EQueryBias QueryBias = EQueryBias::NONE)
	{
		return FVector2D::ZeroVector;
	}

private:

	bool ContainsNode(const TSharedRef<GraphQTNode>& QNode, const UEdGraphNode* GraphNode)
	{
		FVector2D GNTopLeft = FVector2D(GraphNode->NodePosX, GraphNode->NodePosY);
		FVector2D GNBotRight = FVector2D(GraphNode->NodePosX + GraphNode->NodeWidth, GraphNode->NodePosY + GraphNode->NodeHeight);

		// Contained in Viewport.
		if (GNTopLeft.ComponentwiseAllGreaterOrEqual(QNode->TopLeft)
			&& GNBotRight.ComponentwiseAllLessOrEqual(QNode->BotRight))
		{
			return true;
		}

		// Out of Viewport
		return false;
	}

	FVector2D GetSegmentSize(const TSharedRef<GraphQTNode> QNode)
	{
		return QNode->BotRight - QNode->TopLeft;
	}

	TArray<TSharedPtr<GraphQTNode>>& SegmentQNodeSpace(const TSharedRef<GraphQTNode>& QNode)
	{
		if (QNode->Children.Num() > 0)
        return *new TArray<TSharedPtr<GraphQTNode>>();

		float SegmentSpaceX = (QNode->BotRight.X - QNode->TopLeft.X) / 2;
		float SegmentSpaceY = (QNode->BotRight.Y - QNode->TopLeft.Y) / 2;

		for (float X = QNode->TopLeft.X; X < QNode->BotRight.X; X += SegmentSpaceX)
		{
			for (float Y = QNode->TopLeft.Y; Y < QNode->BotRight.Y; Y += SegmentSpaceY)
			{
				FVector2D TopLeft = FVector2D(X, Y);
				FVector2D BotRight = FVector2D(X + SegmentSpaceX, Y + SegmentSpaceY);

				QNode->Children.Add(MakeShared<GraphQTNode>(TSharedRef<GraphQuadTree>(this), TopLeft, BotRight, QNode->Depth++));
			}
		}

		return QNode->Children;
	}

private:

	TSharedPtr<GraphQTNode> RootNode;

	UEdGraph* LinkedGraph;
	TWeakPtr<SGraphEditor> LinkedEditor;

	int8 DepthLimit;
	FVector2D MinSegmentSize;
};