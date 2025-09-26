// Copyright (C) HCI-BCU. All rights reserved.
// Published under GPLv3 License, 2025. See LICENSE in the Plugin Root for more information.

#pragma once

#include "CoreMinimal.h"

#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "GraphEditor.h"
#include "SGraphPanel.h"

#include "GraphQuadTreeNode.h"
#include "AccessionLogging.h"
#include "Rendering/SlateDrawBuffer.h"

class FGraphQuadTree : public TSharedFromThis<FGraphQuadTree>
{
public:
#define _TMP_BIT_OFF(x) 0 << x
	enum class EQueryBias : uint8
	{
		NONE = _TMP_BIT_OFF(0),
		LEFT = _TMP_BIT_OFF(1),
		RIGHT = _TMP_BIT_OFF(2),
		UP = _TMP_BIT_OFF(3),
		DOWN = _TMP_BIT_OFF(4),
	};
#undef _TMP_BIT_OFF

	FGraphQuadTree(UEdGraph *Graph, int8 DepthLimit = 6, FVector2D MinSegmentSizeLimit = FVector2D())
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
			GetPanelPosition(RootTopLeft), GetPanelPosition(RootBotRight));
	}

	FGraphQuadTree(const TSharedPtr<SGraphEditor> &GraphEditor, int8 DepthLimit = 6, FVector2D MinSegmentSizeLimit = FVector2D())
		: LinkedGraph(GraphEditor->GetCurrentGraph()), LinkedEditor(GraphEditor), DepthLimit(DepthLimit), MinSegmentSize(MinSegmentSizeLimit)
	{
		LinkedPanel = GraphEditor->GetGraphPanel();
	}

	/**
	 * Builds the Initial Tree From the Linked Graph, and its Editor.
	 */
	void BuildTree()
	{
		TSharedPtr<SGraphEditor> GraphEditor = LinkedEditor.Pin();

		FVector2D RootTopLeft = LinkedPanel->GetViewOffset();
		FVector2D RootBotRight = RootTopLeft + GraphEditor->GetCachedGeometry().GetLocalSize();

		RootNode = MakeShared<FGraphQTNode>(
			this,
			RootTopLeft, RootBotRight,
			FVector2D::ZeroVector, GraphEditor->GetCachedGeometry().GetLocalSize());

		TSharedPtr<SGraphNode> NodeWidget;
		for (auto &Node : LinkedGraph->Nodes)
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

	/**
	 * Adds a Graph Node into the Quad Tree.
	 * @param NodeToAdd Graph Node to add into the Quad Tree.
	 */
	void AddGraphNode(const UEdGraphNode *NodeToAdd)
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

			for (auto &Child : QNode->PartitionSpace(MinSegmentSize))
			{
				NodesToCheck.Enqueue(Child);
			}
		}
	}

	/**
	 * Adds an Array of Graph Nodes into the Quad Tree.
	 * @param NodesToAdd Array of Graph Nodes to add into the Quad Tree.
	 */
	void AddGraphNodes(TArray<UEdGraphNode *> NodesToAdd)
	{
	}

	/**
	 * Find the Optimal Location in the Viewport for a New Node.
	 * @param QueryBias Directional Bias when Searching for Location.
	 * @return The Found Optimal Location in the Viewport.
	 */
	FVector2D FindOptimalLocation(EQueryBias QueryBias = EQueryBias::NONE)
	{
		return FVector2D::ZeroVector;
	}

	/**
	 * Visualizes the Quad Tree on the Graph Editor.
	 */
	void Visualize()
	{
		if (!LinkedEditor.IsValid())
			return;

		TSharedPtr<SGraphEditor> GraphEditor = LinkedEditor.Pin();

		TSharedPtr<SWindow> GraphWindow = FSlateApplication::Get().FindWidgetWindow(GraphEditor.ToSharedRef());
		if (!GraphWindow.IsValid())
			return;

		const FPaintGeometry PaintGeometry = GraphEditor->GetPaintSpaceGeometry().ToPaintGeometry();

		FSlateRenderer *Renderer = FSlateApplication::Get().GetRenderer();
		if (Renderer == nullptr)
			return;

		FSlateDrawBuffer &DrawBuffer = Renderer->AcquireDrawBuffer();
		FSlateWindowElementList &ElementList = DrawBuffer.AddWindowElementList(GraphWindow.ToSharedRef());

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
				LineColor);
		}

		TArray<FVector2D> LinePoints = TArray<FVector2D>{
			FVector2D::ZeroVector,
			FVector2D::ZeroVector};

		RootNode->Visualize(ElementList, LinePoints, PaintGeometry, ApproxLayerID, LineColor);

		Renderer->DrawWindows(DrawBuffer);

		Renderer->ReleaseDrawBuffer(DrawBuffer);
	}

private:
	/**
	 * Checks if a Graph Node is Contained in a Quad Tree Segment.
	 * @param QNode Quad Tree Segment to Check.
	 * @param GraphNode Graph Node to Check.
	 * @return True, if the Graph Node is Contained in the Quad Tree Segment, otherwise False.
	 */
	bool ContainsNode(const TSharedRef<FGraphQTNode> &QNode, const UEdGraphNode *GraphNode)
	{
		FVector2D GNTopLeft = FVector2D(GraphNode->NodePosX, GraphNode->NodePosY);
		FVector2D GNBotRight = FVector2D(GraphNode->NodePosX + GraphNode->NodeWidth, GraphNode->NodePosY + GraphNode->NodeHeight);

		// Contained in Viewport.
		if (GNTopLeft.ComponentwiseAllGreaterOrEqual(QNode->TopLeft) && GNBotRight.ComponentwiseAllLessOrEqual(QNode->BotRight))
		{
			return true;
		}

		// Out of Viewport
		return false;
	}

	/**
	 * Gets the Size of the provided Quad Tree Segment.
	 * @param QNode Segment to Obtain the Size of.
	 * @return Size of the Quad Tree Segment
	 */
	FVector2D GetSegmentSize(const TSharedRef<FGraphQTNode> &QNode) const
	{
		return QNode->BotRight - QNode->TopLeft;
	}

	/**
	 * Segments the provided Quad Tree Segment, applying descendants.
	 * @param QNode Segment of the Quad Tree, to segment further.
	 * @return Array of Segments descending from the provided Quad Tree Segment.
	 */
	TArray<TSharedPtr<FGraphQTNode>> &SegmentQNodeSpace(const TSharedRef<FGraphQTNode> &QNode)
	{
		if (QNode->ContainsSegments())
			return QNode->Children;

		QNode->Children.Reserve(4);

		const SGraphPanel *GraphPanel = LinkedEditor.Pin()->GetGraphPanel();

		QNode->PartitionSpace(MinSegmentSize);

		return QNode->Children;
	}

	/**
	 * Gets the Elements List of an SWindow, and Acquires the Draw Buffer in the Process.
	 * @param Window Window To Get The Element List From.
	 * @param DrawBuffer Acquired Draw Buffer.
	 * @return Element List of the Window.
	 */
	FSlateWindowElementList *Vis_GetWindowElementList(const TSharedRef<SWindow> &Window, FSlateDrawBuffer *DrawBuffer)
	{
		FSlateRenderer *Renderer = FSlateApplication::Get().GetRenderer();
		if (Renderer == nullptr)
			return nullptr;

		DrawBuffer = &Renderer->AcquireDrawBuffer();
		if (DrawBuffer == nullptr)
		{
			UE_LOG(LogAccession, Log, TEXT("Draw Buffer Couldn't be Acquired."))
			return nullptr;
		}

		// The Add Function First Searches For a Pre-Existing Element List
		return &DrawBuffer->AddWindowElementList(Window);
	}

	/**
	 * Approximates the Layer Index of the Provided Widget, through its Window Path.
	 * @param Widget Widget To Approximate the Layer from.
	 * @return Approximate Layer of the Provided Widget.
	 */
	const int32 GetApproximateLayer(const TSharedRef<const SWidget> &Widget) const
	{
		FWidgetPath WidgetPath;
		FSlateApplication::Get().FindPathToWidget(Widget, WidgetPath);

		int32 LayerID = WidgetPath.Widgets.FindLastByPredicate([&](const FArrangedWidget &ArrangedWidget)
															   { return ArrangedWidget.Widget == Widget; });

		return LayerID != INDEX_NONE ? LayerID : 0;
	}

	/**
	 * Gets the Panel Position from a Graph Position.
	 * @param GraphPosition Graph Position to Convert.
	 * @return Converted Position in the Panel, based on the Graph Position.
	 */
	FVector2D GetPanelPosition(const FVector2D &GraphPosition) const
	{
		return (GraphPosition - LinkedPanel->GetViewOffset()) * LinkedPanel->GetZoomAmount();
	}

private:
	TSharedPtr<FGraphQTNode> RootNode;

	UEdGraph *LinkedGraph;
	TWeakPtr<SGraphEditor> LinkedEditor;
	SGraphPanel *LinkedPanel;

	int8 DepthLimit;
	FVector2D MinSegmentSize;
};
