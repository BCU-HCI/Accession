// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "GraphEditor.h"
#include "SGraphPanel.h"

#include "GraphQuadTreeNode.h"
#include "OpenAccessibilityLogging.h"
#include "Rendering/SlateDrawBuffer.h"

class FGraphQuadTree : public TSharedFromThis<FGraphQuadTree>
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

	FGraphQuadTree(UEdGraph* Graph, int8 DepthLimit = 6, FVector2D MinSegmentSizeLimit = FVector2D())
	: LinkedGraph(Graph), LinkedEditor(nullptr), DepthLimit(DepthLimit), MinSegmentSize(MinSegmentSizeLimit)
	{
		TSharedPtr<SGraphEditor> GraphEditor = SGraphEditor::FindGraphEditorForGraph(Graph);
		LinkedEditor = GraphEditor;

		LinkedPanel = GraphEditor->GetGraphPanel();

		FVector2D RootTopLeft = LinkedPanel->GetViewOffset();
		FVector2D RootBotRight = RootTopLeft + LinkedPanel->GetCachedGeometry().GetLocalSize();

		RootNode = MakeShared<FGraphQTNode>(
			this, 
			RootTopLeft, RootBotRight,
			GetPanelPosition(RootTopLeft), GetPanelPosition(RootBotRight)
		);
	}

	FGraphQuadTree(const TSharedPtr<SGraphEditor>& GraphEditor, int8 DepthLimit = 6, FVector2D MinSegmentSizeLimit = FVector2D())
	: LinkedGraph(GraphEditor->GetCurrentGraph()), LinkedEditor(GraphEditor), DepthLimit(DepthLimit), MinSegmentSize(MinSegmentSizeLimit)
	{
		LinkedPanel = GraphEditor->GetGraphPanel();
	}

	// Builds the Initial Tree from the Linked Graph
	void BuildTree()
	{
		TSharedPtr<SGraphEditor> GraphEditor = LinkedEditor.Pin();

		FVector2D RootTopLeft = LinkedPanel->GetViewOffset();
		FVector2D RootBotRight = RootTopLeft + GraphEditor->GetCachedGeometry().GetLocalSize();

		RootNode = MakeShared<FGraphQTNode>(
			this,
			RootTopLeft, RootBotRight,
			FVector2D::ZeroVector, GraphEditor->GetCachedGeometry().GetLocalSize()
		);

		TSharedPtr<SGraphNode> NodeWidget;
		for (auto& Node : LinkedGraph->Nodes)
		{
			NodeWidget = LinkedPanel->GetNodeWidgetFromGuid(Node->NodeGuid);
			if (!NodeWidget.IsValid())
				continue;


			FVector2D NodeTopLeft(Node->NodePosX, Node->NodePosY);
			FVector2D NodeBotRight(Node->NodePosX + Node->NodeWidth, Node->NodePosY + Node->NodeHeight);

			if (RootTopLeft.ComponentwiseAllLessOrEqual(NodeTopLeft) && RootBotRight.ComponentwiseAllGreaterOrEqual(NodeTopLeft))
				AddGraphNode(Node);
		}
	}

	void AddGraphNode(const UEdGraphNode* NodeToAdd)
	{
		if (!RootNode.IsValid() || NodeToAdd == nullptr)
			return;

		FVector2D GNTopLeft(NodeToAdd->NodePosX, NodeToAdd->NodePosY);
		FVector2D GNBotRight(NodeToAdd->NodePosX + NodeToAdd->NodeWidth, NodeToAdd->NodePosY + NodeToAdd->NodeHeight);

		TQueue<TSharedPtr<FGraphQTNode>> NodesToCheck = TQueue<TSharedPtr<FGraphQTNode>>();
		NodesToCheck.Enqueue(RootNode);

		TSharedPtr<FGraphQTNode> QNode;
		while (!NodesToCheck.IsEmpty())
		{
			NodesToCheck.Dequeue(QNode);
			if (!QNode.IsValid())
				continue;

			// QuadTreeNode Doesn't Contain the Nodes Rect.
			if (!QNode->ContainsNodeRect(GNTopLeft, GNBotRight))
				continue;

			QNode->ContainedNodes.Add(NodeToAdd);

			if (QNode->Depth >= DepthLimit)
				return;

			if (!QNode->ContainsSegments())
				QNode->PartitionSpace(MinSegmentSize);

			for (auto& Child : QNode->PartitionSpace(MinSegmentSize))
			{
				NodesToCheck.Enqueue(Child);
			}
		}

		/*
		FVector2D GNTopLeft = FVector2D(GraphNode->NodePosX, GraphNode->NodePosY);
		FVector2D GNBotRight = FVector2D(GraphNode->NodePosX + GraphNode->NodeWidth, GraphNode->NodePosY + GraphNode->NodeHeight);


		TSet<TSharedRef<FGraphQTNode>> CheckedNodes;
		TQueue<TSharedPtr<FGraphQTNode>> NodesToCheck = TQueue<TSharedPtr<FGraphQTNode>>();
		NodesToCheck.Enqueue(RootNode.ToSharedRef());

		TSharedPtr<FGraphQTNode> QNode;

		while (!NodesToCheck.IsEmpty())
		{
			NodesToCheck.Dequeue(QNode);
			CheckedNodes.Add(QNode.ToSharedRef());

			if (!QNode->ContainsNodeRect(GNTopLeft, GNBotRight))
				continue;

			QNode->ContainedNodes.Add(GraphNode);

			if (QNode->Depth <= DepthLimit && GetSegmentSize(QNode.ToSharedRef()) > MinSegmentSize)
			{
				TArray<TSharedPtr<FGraphQTNode>> ChildNodes;
				if (QNode->ContainsSegments())
					ChildNodes = QNode->Children;
				else
					ChildNodes = SegmentQNodeSpace(QNode.ToSharedRef());

				for (auto GraphQtNode : ChildNodes)
				{
					NodesToCheck.Enqueue(GraphQtNode.ToSharedRef());
				}				
			}
		}
		*/
	}

	void AddGraphNodes(TArray<UEdGraphNode*> NodesToAdd)
	{
		
	}

	FVector2D FindOptimalLocation(EQueryBias QueryBias = EQueryBias::NONE)
	{
		return FVector2D::ZeroVector;
	}

	void Visualize()
	{
		if (!LinkedEditor.IsValid())
			return;

		TSharedPtr<SGraphEditor> GraphEditor = LinkedEditor.Pin();

		TSharedPtr<SWindow> GraphWindow = FSlateApplication::Get().FindWidgetWindow(GraphEditor.ToSharedRef());
		if (!GraphWindow.IsValid())
			return;

		const FPaintGeometry PaintGeometry = GraphEditor->GetPaintSpaceGeometry().ToPaintGeometry();

		FSlateRenderer* Renderer = FSlateApplication::Get().GetRenderer();
		if (Renderer == nullptr)
			return;

		FSlateDrawBuffer& DrawBuffer = Renderer->AcquireDrawBuffer();
		FSlateWindowElementList& ElementList = DrawBuffer.AddWindowElementList(GraphWindow.ToSharedRef());

		const int32 ApproxLayerID = GetApproximateLayer(GraphEditor->AsShared()) + 1;
		const FLinearColor LineColor = FLinearColor::Green;

		// Draw Main Outer-Box
		{
			FSlateDrawElement::MakeBox(
				ElementList,
				ApproxLayerID,
				PaintGeometry,
				FCoreStyle::Get().GetBrush("Debug.Border"),
				ESlateDrawEffect::None,
				LineColor
			);
		}

		TArray<FVector2D> LinePoints = TArray<FVector2D> {
			FVector2D::ZeroVector,
			FVector2D::ZeroVector
		};

		RootNode->Visualize(ElementList, LinePoints, PaintGeometry, ApproxLayerID, LineColor);

		Renderer->DrawWindows(DrawBuffer);

		Renderer->ReleaseDrawBuffer(DrawBuffer);
	}

private:

	bool ContainsNode(const TSharedRef<FGraphQTNode>& QNode, const UEdGraphNode* GraphNode)
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

	FVector2D GetSegmentSize(const TSharedRef<FGraphQTNode>& QNode) const
	{
		return QNode->BotRight - QNode->TopLeft;
	}

	TArray<TSharedPtr<FGraphQTNode>>& SegmentQNodeSpace(const TSharedRef<FGraphQTNode>& QNode)
	{
		if (QNode->ContainsSegments())
			return QNode->Children;

		QNode->Children.Reserve(4);

		const SGraphPanel* GraphPanel = LinkedEditor.Pin()->GetGraphPanel();

		QNode->PartitionSpace(MinSegmentSize);

		return QNode->Children;
	}

	FPaintGeometry Vis_GetPaintGeometry(const SWidget* Widget) const
	{
		const FGeometry Geometry = Widget->GetTickSpaceGeometry();

		return FPaintGeometry(
			Geometry.GetAccumulatedLayoutTransform(),
			Geometry.GetAccumulatedRenderTransform(),
			Geometry.GetLocalSize(),
			Geometry.HasRenderTransform()
		);
	}

	FSlateWindowElementList* Vis_GetWindowElementList(const TSharedRef<SWindow>& Window, FSlateDrawBuffer* DrawBuffer)
	{
		FSlateRenderer* Renderer = FSlateApplication::Get().GetRenderer();
		if (Renderer == nullptr)
			return nullptr;

		DrawBuffer = &Renderer->AcquireDrawBuffer();
		if (DrawBuffer == nullptr)
		{
			UE_LOG(LogOpenAccessibility, Log, TEXT("Draw Buffer Couldn't be Acquired."))
			return nullptr;
		}

		// The Add Function First Searches For a Pre-Existing Element List
		return &DrawBuffer->AddWindowElementList(Window);
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

	TSharedPtr<FGraphQTNode> RootNode;

	UEdGraph* LinkedGraph;
	TWeakPtr<SGraphEditor> LinkedEditor;
	SGraphPanel* LinkedPanel;

	int8 DepthLimit;
	FVector2D MinSegmentSize;
};
