// Fill out your copyright notice in the Description page of Project Settings.


#include "OAccessibilityNodeFactory.h"
#include "OpenAccessibilityLogging.h"

#include "Logging/StructuredLog.h"

#include "NodeFactory.h"
#include "EdGraphUtilities.h"

#include "Styling/AppStyle.h"
#include "SNodePanel.h"
#include "SGraphNode.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#include "OpenAccessibility.h"

FAccessibilityNodeFactory::FAccessibilityNodeFactory() : FGraphPanelNodeFactory()
{
	UE_LOGFMT(LogOpenAccessibility, Display, "Accessibility Node Factory Constructed");
}

FAccessibilityNodeFactory::~FAccessibilityNodeFactory()
{

}

TSharedPtr<class SGraphNode> FAccessibilityNodeFactory::CreateNode(UEdGraphNode* InNode) const
{
    UE_LOG(LogOpenAccessibility, Display, TEXT("Accessibility Node Factory Used to construct %s"), *InNode->GetName());

    check(InNode);

    // Hack to get around the possible infinite loop of using 
    // this factory to create the node from the factory itself.
    FEdGraphUtilities::UnregisterVisualNodeFactory(FOpenAccessibilityModule::Get().AccessibilityNodeFactory);
    TSharedPtr<SGraphNode> OutNode = FNodeFactory::CreateNodeWidget(InNode);
    FEdGraphUtilities::RegisterVisualNodeFactory(FOpenAccessibilityModule::Get().AccessibilityNodeFactory);

    // Get Node Accessibility Index, from registry
    TSharedRef<FGraphIndexer> GraphIndexer = FOpenAccessibilityModule::Get()
        .AssetAccessibilityRegistry->GetGraphIndexer(InNode->GetGraph());

    int NodeIndex = GraphIndexer->ContainsNode(InNode);
    if (NodeIndex == -1)
    {
        UE_LOG(LogOpenAccessibility, Error, TEXT("Node Not Found In Graph Indexer"));
    }

    // Build Accessibility Widget Wrapper.

    // Get the Slot Outputted from the factory.
    TSharedRef<SWidget> WidgetToWrap = OutNode->GetSlot(ENodeZone::Center)->GetWidget();
    check(WidgetToWrap != SNullWidget::NullWidget);

    // Rebuild the Widget with a wrapper containing accessibility information.
    OutNode->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Fill)
        [
			SNew(SOverlay)

                + SOverlay::Slot()
                [
                    SNew(SImage)
                        .Image(FAppStyle::GetBrush("Graph.Node.Body"))
                        .ColorAndOpacity(OutNode.Get(), &SGraphNode::GetNodeBodyColor)
                ]

                + SOverlay::Slot()
                [
                    SNew(SVerticalBox)

                        + SVerticalBox::Slot()
                        .HAlign(HAlign_Fill)
                        .Padding(FMargin(3.f, 0.5f))
                        [
                            // Accessibility Content
                            SNew(SHorizontalBox)
                                + SHorizontalBox::Slot()
                                .HAlign(HAlign_Right)
                                .VAlign(VAlign_Center)
                                [
                                    SNew(SOverlay)
                                        + SOverlay::Slot()
                                        [
                                            SNew(STextBlock)
                                                .Text(FText::FromString("[" + FString::FromInt(NodeIndex) + "]"))
                                        ]
                                ]
						]

						+ SVerticalBox::Slot()
						.HAlign(HAlign_Fill)
						.AutoHeight()
                        [
							WidgetToWrap
						]
				]
		];

    return OutNode;
}