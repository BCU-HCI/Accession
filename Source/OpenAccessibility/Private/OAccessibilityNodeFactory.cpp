// Fill out your copyright notice in the Description page of Project Settings.


#include "OAccessibilityNodeFactory.h"
#include "OpenAccessibilityLogging.h"

#include "Logging/StructuredLog.h"

#include "NodeFactory.h"
#include "EdGraphUtilities.h"

#include "Styling/AppStyle.h"
#include "SGraphPanel.h"
#include "SNodePanel.h"
#include "SGraphNode.h"
#include "SGraphPin.h"
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

    int NodeIndex = -1;
    GraphIndexer->GetOrAddNode(InNode, NodeIndex);

    {
        // Create Accessibility Widgets For Pins
        TArray<UEdGraphPin*> Pins = InNode->GetAllPins();
        TSharedPtr<SGraphPin> PinWidget;

        for (int i = 0; i < Pins.Num(); i++)
        {
            UEdGraphPin* Pin = Pins[i];

            PinWidget = OutNode->FindWidgetForPin(Pin);
            if (!PinWidget.IsValid())
            {
				continue;
			}

            WrapPinWidget(Pin, PinWidget.ToSharedRef(), i, OutNode.Get());
        }

        PinWidget.Reset();
    }

    // Wrap The Node Widget
    WrapNodeWidget(InNode, OutNode.ToSharedRef(), NodeIndex);

    return OutNode;
}

void FAccessibilityNodeFactory::WrapNodeWidget(UEdGraphNode* Node, TSharedRef<SGraphNode> NodeWidget, int NodeIndex) const
{
    TSharedRef<SWidget> WidgetToWrap = NodeWidget->GetSlot(ENodeZone::Center)->GetWidget();
    check(WidgetToWrap != SNullWidget::NullWidget);

    NodeWidget->GetOrAddSlot(ENodeZone::Center)
        .HAlign(HAlign_Fill)
        [
            SNew(SVerticalBox)

                + SVerticalBox::Slot()
                .HAlign(HAlign_Fill)
                .AutoHeight()
                .Padding(FMargin(1.5f, 0.25f))
                [
                    SNew(SOverlay)

                        + SOverlay::Slot()
                        [
                            SNew(SImage)
                                .Image(FAppStyle::Get().GetBrush("Graph.Node.Body"))
                        ]

                        + SOverlay::Slot()
                        .Padding(FMargin(4.0f, 0.0f))
                        [
                            SNew(SHorizontalBox)
                                + SHorizontalBox::Slot()
                                .HAlign(HAlign_Right)
                                .VAlign(VAlign_Center)
                                .Padding(1.f)
                                [
                                    SNew(SOverlay)
                                        + SOverlay::Slot()
                                        [
                                            SNew(STextBlock)
                                                .Text(FText::FromString("[" + FString::FromInt(NodeIndex) + "]"))
                                        ]
                                ]
                        ]
                ]

                + SVerticalBox::Slot()
                .HAlign(HAlign_Fill)
                .AutoHeight()
                [
                    WidgetToWrap
                ]
        ];
}

void FAccessibilityNodeFactory::WrapPinWidget(UEdGraphPin* Pin, TSharedRef<SGraphPin> PinWidget, int PinIndex, SGraphNode* OwnerNode) const
{
    TSharedRef<SWidget> PinWidgetContent = PinWidget->GetContent();
    check(PinWidgetContent != SNullWidget::NullWidget);

    TSharedRef<SWidget> AccessibilityWidget = SNew(SOverlay)
        .Visibility_Lambda([OwnerNode]() -> EVisibility {

            if (OwnerNode->HasAnyUserFocusOrFocusedDescendants() || OwnerNode->IsHovered() || OwnerNode->GetOwnerPanel()->SelectionManager.IsNodeSelected(OwnerNode->GetNodeObj()))
                return EVisibility::Visible;

            return EVisibility::Collapsed;
        })
        + SOverlay::Slot()
        [
            SNew(STextBlock)
                .Text(FText::FromString("[" + FString::FromInt(PinIndex) + "]"))
        ];

    switch (Pin->Direction)
    {
        case EEdGraphPinDirection::EGPD_Input:
        {
            PinWidget->SetContent(
                SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        PinWidgetContent
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        AccessibilityWidget
                    ]
            );

            break;
        }

        case EEdGraphPinDirection::EGPD_Output:
        {
            PinWidget->SetContent(
                SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        AccessibilityWidget
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        PinWidgetContent
                    ]
            );
            break;
		}

        default:
        {
            UE_LOG(LogOpenAccessibility, Error, TEXT("Pin Direction Not Recognized"));
            break;
        }
    }
}