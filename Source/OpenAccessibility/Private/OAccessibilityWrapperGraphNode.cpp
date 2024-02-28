// Copyright F-Dudley. All Rights Reserved.


#include "OAccessibilityWrapperGraphNode.h"

OAccessibilityWrapperGraphNode::OAccessibilityWrapperGraphNode()
{
}

OAccessibilityWrapperGraphNode::~OAccessibilityWrapperGraphNode()
{
}

void OAccessibilityWrapperGraphNode::Construct(const FArguments& InArgs, UEdGraphNode* InNode)
{
	GraphNode = InNode;


	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			//.BorderImage(FAppStyle::Get().GetBrush("Graph.Node.Body"))
			.Padding(0)
				[
				SNew(SOverlay)

				// Add Possible Image Background for Nodes.
				+ SOverlay::Slot()
					[
					SNew(SImage)
					//.Image(FAppStyle::Get().GetBrush("Graph.Node.ColorSpill"))
				]

				+ SOverlay::Slot()
					[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
						[
						SNew(STextBlock)
						.Text(FText::FromString("[0]"))
					]
					/* + SVerticalBox::Slot()
					.AutoHeight()
						[
						InArgs._NodeToWrap
					]*/
				]
			]
		];
}
