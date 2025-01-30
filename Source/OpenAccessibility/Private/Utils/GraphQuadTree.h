// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "GraphEditor.h"
#include "SGraphPanel.h"

#include "GraphQuadTreeNode.h"
#include "Rendering/SlateDrawBuffer.h"

class GraphQuadTree : public TSharedFromThis<GraphQuadTree>
{
public:

#define BIT_OFF(x) 0 << x
	enum class EQueryBias : uint8
	{
		NONE = BIT_OFF(0),
		LEFT = BIT_OFF(1),
		RIGHT = BIT_OFF(2),
		UP = BIT_OFF(3),
		DOWN = BIT_OFF(4),
	};
#undef BIT_OFF

	GraphQuadTree(UEdGraph* Graph, int8 DepthLimit = 6, FVector2D MinSegmentSizeLimit = FVector2D())
	: LinkedGraph(Graph), LinkedEditor(nullptr), DepthLimit(DepthLimit), MinSegmentSize(MinSegmentSizeLimit)
	{
		TSharedPtr<SGraphEditor> GraphEditor = SGraphEditor::FindGraphEditorForGraph(Graph);
		LinkedEditor = GraphEditor;

		LinkedPanel = GraphEditor->GetGraphPanel();

		FVector2D RootTopLeft = LinkedPanel->GetViewOffset();
		FVector2D RootBotRight = RootTopLeft + LinkedPanel->GetCachedGeometry().GetLocalSize();

		RootNode = MakeShared<GraphQTNode>(TSharedRef<GraphQuadTree>(this), RootTopLeft, RootBotRight);
	}

	GraphQuadTree(const TSharedPtr<SGraphEditor>& GraphEditor, int8 DepthLimit = 6, FVector2D MinSegmentSizeLimit = FVector2D())
	: LinkedGraph(GraphEditor->GetCurrentGraph()), LinkedEditor(GraphEditor), DepthLimit(DepthLimit), MinSegmentSize(MinSegmentSizeLimit)
	{
		LinkedPanel = GraphEditor->GetGraphPanel();

		FVector2D RootTopLeft = LinkedPanel->GetViewOffset();
		FVector2D RootBotRight = RootTopLeft + LinkedPanel->GetCachedGeometry().GetLocalSize();

		RootNode = MakeShared<GraphQTNode>(TSharedRef<GraphQuadTree>(this), RootTopLeft, RootBotRight);
	}

	void AddGraphNode(const UEdGraphNode* GraphNode)
	{
		if (!RootNode.IsValid() || GraphNode == nullptr)
			return;

		FVector2D GNTopLeft = FVector2D(GraphNode->NodePosX, GraphNode->NodePosY);
		FVector2D GNBotRight = FVector2D(GraphNode->NodePosX + GraphNode->NodeWidth, GraphNode->NodePosY + GraphNode->NodeHeight);


		TSet<TSharedRef<GraphQTNode>> CheckedNodes;
		TQueue<TSharedRef<GraphQTNode>> NodesToCheck;
		NodesToCheck.Enqueue(RootNode.ToSharedRef());

		TSharedRef<GraphQTNode> QNode;

		while (!NodesToCheck.IsEmpty())
		{
			NodesToCheck.Dequeue(QNode);
			CheckedNodes.Add(QNode);

			if (!QNode->ContainsNodeRect(GNTopLeft, GNBotRight))
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
	}

	FVector2D FindOptimalLocation(EQueryBias QueryBias = EQueryBias::NONE)
	{
		return FVector2D::ZeroVector;
	}

	void Visualize() const
	{
		if (!LinkedEditor.IsValid())
			return;

		TSharedPtr<SGraphEditor> GraphEditor = LinkedEditor.Pin();
		SGraphPanel* GraphPanel = GraphEditor->GetGraphPanel();

		TSharedPtr<SWindow> GraphWindow = FSlateApplication::FindWidgetWindow(GraphEditor.ToSharedRef());
		if (!GraphWindow.IsValid())
			return;

		const FPaintGeometry PaintGeometry = Vis_GetPaintGeometry(GraphPanel);

		FSlateWindowElementList* ElementList = Vis_GetWindowElementList(GraphWindow.ToSharedRef());
		if (ElementList == nullptr)
			return;

		const int32 ApproxLayerID = GetApproximateLayer(GraphPanel->AsShared()) + 1;

		const FLinearColor LineColor = FLinearColor::Green;


		// Draw Main Outer-Box
		{
			FSlateDrawElement::MakeBox(
				*ElementList,
				ApproxLayerID,
				PaintGeometry,
				FCoreStyle::Get().GetBrush("Debug.Border"),
				ESlateDrawEffect::None,
				LineColor
			);			
		}


		// Draw Inner Lines
		{
			TArray<TSharedPtr<GraphQTNode>> NodesToDraw {
				RootNode->Children
			};

			TArray<FVector2D> LinePoints = TArray<FVector2D>();
			LinePoints.Reserve(2);

			while (!NodesToDraw.IsEmpty())
			{
				TSharedPtr<GraphQTNode> QNode = NodesToDraw.Pop();

				if (QNode->Children.Num() == 0)
					continue;

				/*
				FVector2D HalfVec = (QNode->BotRight - QNode->TopLeft) / 2;

                // Add Vertical Line
				LinePoints[0] = FVector2D(QNode->TopLeft.X + HalfVec.X, QNode->TopLeft.Y);
				LinePoints[1] = FVector2D(QNode->TopLeft.X + HalfVec.X, QNode->BotRight.Y);

				FSlateDrawElement::MakeLines(
					*ElementList,
					ApproxLayerID,
					PaintGeometry,
					LinePoints,
					ESlateDrawEffect::None,
					LineColor
				);


                // Add Horizontal Line
				LinePoints[0] = FVector2D(QNode->TopLeft.X, QNode->TopLeft.Y + HalfVec.Y);
				LinePoints[1] = FVector2D(QNode->BotRight.X, QNode->TopLeft.Y + HalfVec.Y);

				FSlateDrawElement::MakeLines(
					*ElementList,
					ApproxLayerID,
					PaintGeometry,
					LinePoints
				);
				*/

				QNode->Visualize(*ElementList, PaintGeometry, ApproxLayerID, LineColor);
			}
		}
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

	FVector2D GetSegmentSize(const TSharedRef<GraphQTNode>& QNode) const
	{
		return QNode->BotRight - QNode->TopLeft;
	}

	TArray<TSharedPtr<GraphQTNode>>& SegmentQNodeSpace(const TSharedRef<GraphQTNode>& QNode)
	{
		if (QNode->ContainsSegments())
			return QNode->Children;

		float SegmentSpaceX = (QNode->BotRight.X - QNode->TopLeft.X) / 2;
		float SegmentSpaceY = (QNode->BotRight.Y - QNode->TopLeft.Y) / 2;

		QNode->Children.Reserve(4);

		const SGraphPanel* GraphPanel = LinkedEditor.Pin()->GetGraphPanel();

		const FVector2D PanelOffset = GraphPanel->GetViewOffset();
		const float PanelZoom = GraphPanel->GetZoomAmount();

		for (float X = QNode->TopLeft.X; X < QNode->BotRight.X; X += SegmentSpaceX)
		{
			for (float Y = QNode->TopLeft.Y; Y < QNode->BotRight.Y; Y += SegmentSpaceY)
			{
				FVector2D TopLeft = FVector2D(X, Y);
				FVector2D BotRight = FVector2D(X + SegmentSpaceX, Y + SegmentSpaceY);

				QNode->Children.Add(MakeShared<GraphQTNode>(TSharedRef<GraphQuadTree>(this), TopLeft, BotRight, QNode->Depth + 1));
			}
		}

		return QNode->Children;
	}

	FPaintGeometry Vis_GetPaintGeometry(const SWidget* Widget) const
	{
		const FGeometry Geometry = Widget->GetPaintSpaceGeometry();

		return FPaintGeometry(
			Geometry.GetAccumulatedLayoutTransform(),
			Geometry.GetAccumulatedRenderTransform(),
			Geometry.GetLocalSize(),
			Geometry.HasRenderTransform()
		);
	}

	FSlateWindowElementList* Vis_GetWindowElementList(const TSharedRef<SWindow>& Window) const
	{
		FSlateRenderer* Renderer = FSlateApplication::Get().GetRenderer();
		if (Renderer == nullptr)
			return nullptr;

		FSlateDrawBuffer& DrawBuffer = Renderer->AcquireDrawBuffer();

		// The Add Function First Searches For a Pre-Existing Element List
		return &DrawBuffer.AddWindowElementList(Window);
	}

	const int32 GetApproximateLayer(const TSharedRef<const SWidget>& Widget) const
	{
		FWidgetPath WidgetPath;
		FSlateApplication::Get().FindPathToWidget(Widget, WidgetPath);

        int32 LayerID = WidgetPath.Widgets.FindLastByPredicate([&](const FArrangedWidget& ArrangedWidget)
        {
        	return ArrangedWidget.Widget == Widget;
        });


		return LayerID != INDEX_NONE ? LayerID : 0;
	}

	FVector2D GetPanelPosition(const FVector2D& GraphPosition) const
	{
		return (GraphPosition - LinkedPanel->GetViewOffset()) * LinkedPanel->GetZoomAmount();
	}

private:

	TSharedPtr<GraphQTNode> RootNode;

	UEdGraph* LinkedGraph;
	TWeakPtr<SGraphEditor> LinkedEditor;
	SGraphPanel* LinkedPanel;

	int8 DepthLimit;
	FVector2D MinSegmentSize;
};
