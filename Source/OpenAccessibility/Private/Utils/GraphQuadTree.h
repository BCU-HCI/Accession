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

	GraphQuadTree(UEdGraph* Graph)
	: LinkedGraph(Graph), LinkedEditor(nullptr)
	{
		TSharedPtr<SGraphEditor> GraphEditor = SGraphEditor::FindGraphEditorForGraph(Graph);
		LinkedEditor = GraphEditor;

		SGraphPanel* GraphPanel = GraphEditor->GetGraphPanel();

		FVector2D RootTopLeft = GraphPanel->GetViewOffset();
		FVector2D RootBotRight = RootTopLeft + GraphPanel->GetCachedGeometry().GetLocalSize();

		RootNode = MakeShared<GraphQTNode>(TSharedRef<GraphQuadTree>(this), RootTopLeft, RootBotRight);
	}

	GraphQuadTree(const TSharedPtr<SGraphEditor>& GraphEditor)
	: LinkedGraph(GraphEditor->GetCurrentGraph()), LinkedEditor(GraphEditor)
	{
		SGraphPanel* GraphPanel = GraphEditor->GetGraphPanel();

		FVector2D RootTopLeft = GraphPanel->GetViewOffset();
		FVector2D RootBotRight = RootTopLeft + GraphPanel->GetCachedGeometry().GetLocalSize();

		RootNode = MakeShared<GraphQTNode>(TSharedRef<GraphQuadTree>(this), RootTopLeft, RootBotRight);
	}

	void AddGraphNode(const UEdGraphNode* GraphNode)
	{
		if (!RootNode.IsValid())
			return;

		// Recursive Check For What QNodes The GraphNode TopLeft and BotRight ar Contained In.
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

	void SegmentQNodeSpace(const TSharedRef<GraphQTNode>& QNode)
	{
		if (QNode->Children.Num() > 0)
			return;

		float SegmentSpaceX = (QNode->BotRight.X - QNode->TopLeft.X) / 2;
		float SegmentSpaceY = (QNode->BotRight.Y - QNode->TopLeft.Y) / 2;

		for (float X = QNode->TopLeft.X; X < QNode->BotRight.X; X += SegmentSpaceX)
		{
			for (float Y = QNode->TopLeft.Y; Y < QNode->BotRight.Y; Y += SegmentSpaceY)
			{
				FVector2D TopLeft = FVector2D(X, Y);
				FVector2D BotRight = FVector2D(X + SegmentSpaceX, Y + SegmentSpaceY);

				QNode->Children.Add(MakeShared<GraphQTNode>(TSharedRef<GraphQuadTree>(this), TopLeft, BotRight));
			}
		}
	}

private:

	TSharedPtr<GraphQTNode> RootNode;

	UEdGraph* LinkedGraph;
	TWeakPtr<SGraphEditor> LinkedEditor;
};