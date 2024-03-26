// Fill out your copyright notice in the Description page of Project Settings.


#include "OAccessibilityNodeFactory.h"
#include "OpenAccessibilityLogging.h"

#include "Logging/StructuredLog.h"

#include "NodeFactory.h"
#include "EdGraphUtilities.h"

#include "Styling/AppStyle.h"
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

    int NodeIndex = GraphIndexer->ContainsNode(InNode);
    if (NodeIndex == -1)
    {
        UE_LOG(LogOpenAccessibility, Error, TEXT("Node Not Found In Graph Indexer"));
    }

    {
        // Create Accessibility Widgets For Pins
        TArray<UEdGraphPin*> Pins = InNode->GetAllPins();

        for (int i = 0; i < Pins.Num(); i++)
        {
            UEdGraphPin* Pin = Pins[i];

            TSharedPtr<SGraphPin> PinWidget = OutNode->FindWidgetForPin(Pin);
            if (!PinWidget.IsValid())
            {
				UE_LOG(LogOpenAccessibility, Warning, TEXT("Pin Widget Not Found"));
				continue;
			}

            WrapPinWidget(Pin, PinWidget.ToSharedRef(), i, OutNode.ToSharedRef());
        }
    }

    // Wrap The Node Widget
    WrapNodeWidget(InNode, OutNode.ToSharedRef(), NodeIndex);

    return OutNode;
}

void FAccessibilityNodeFactory::WrapNodeWidget(UEdGraphNode* Node, TSharedRef<SGraphNode> NodeWidget, int NodeIndex) const
{
    TSharedPtr<SWidget> WidgetToWrap = NodeWidget->GetSlot(ENodeZone::Center)->GetWidget();
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
                    WidgetToWrap.ToSharedRef()
                ]
        ];
}

void FAccessibilityNodeFactory::WrapPinWidget(UEdGraphPin* Pin, TSharedRef<SGraphPin> PinWidget, int PinIndex, TSharedRef<SGraphNode> OwnerNode) const
{
    TSharedPtr<SWidget> PinWidgetContent = PinWidget->GetContent();
    check(PinWidgetContent != SNullWidget::NullWidget);

    TSharedPtr<SWidget> AccessibilityWidget = SNew(SOverlay)
        .Visibility_Lambda([OwnerNode]() -> EVisibility {
            
            TOptional<EFocusCause> NodeVisability = OwnerNode->HasAnyUserFocus();
            if (NodeVisability.IsSet() && NodeVisability.GetValue() != EFocusCause::OtherWidgetLostFocus)
                return EVisibility::Visible;

            else if (OwnerNode->HasAnyUserFocusOrFocusedDescendants() || OwnerNode->IsHovered())
                return EVisibility::Visible;

            return EVisibility::Collapsed;
        })
        + SOverlay::Slot()
        [
            SNew(STextBlock)
                .Text(FText::FromString("[" + FString::FromInt(PinIndex) + "]"))
        ];

    TSharedRef<SWidget> WrappedWidget = SNullWidget::NullWidget;

    switch (Pin->Direction)
    {
        case EEdGraphPinDirection::EGPD_Input:
        {
            PinWidget->SetContent(
                SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        PinWidgetContent.ToSharedRef()
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        AccessibilityWidget.ToSharedRef()
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
                        AccessibilityWidget.ToSharedRef()
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        PinWidgetContent.ToSharedRef()
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