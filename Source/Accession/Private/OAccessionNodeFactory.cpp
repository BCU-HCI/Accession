// Copyright (C) HCI-BCU 2025. All rights reserved.

#include "OAccessionNodeFactory.h"
#include "AccessionLogging.h"

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

#include "Accession.h"
#include "Widgets/SIndexer.h"

FAccessionNodeFactory::FAccessionNodeFactory() : FGraphPanelNodeFactory()
{
    UE_LOGFMT(LogAccession, Display, "Accession Node Factory Constructed");
}

FAccessionNodeFactory::~FAccessionNodeFactory()
{
}

TSharedPtr<class SGraphNode> FAccessionNodeFactory::CreateNode(UEdGraphNode *InNode) const
{
    UE_LOG(LogAccession, Display, TEXT("Accession Node Factory Used to construct %s"), *InNode->GetName());

    check(InNode);

    // Hack to get around the possible infinite loop of using
    // this factory to create the node from the factory itself.
    FEdGraphUtilities::UnregisterVisualNodeFactory(FAccessionModule::Get().NodeFactory);
    TSharedPtr<SGraphNode> OutNode = FNodeFactory::CreateNodeWidget(InNode);
    FEdGraphUtilities::RegisterVisualNodeFactory(FAccessionModule::Get().NodeFactory);

    // Get Node Index, from registry
    TSharedRef<FGraphIndexer> GraphIndexer = FAccessionModule::Get()
                                                 .AssetRegistry->GetGraphIndexer(InNode->GetGraph());

    int NodeIndex = -1;
    GraphIndexer->GetOrAddNode(InNode, NodeIndex);

    {
        // Create Index Widgets For Pins
        TArray<UEdGraphPin *> Pins = InNode->GetAllPins();
        TSharedPtr<SGraphPin> PinWidget;

        for (int i = 0; i < Pins.Num(); i++)
        {
            UEdGraphPin *Pin = Pins[i];

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

void FAccessionNodeFactory::WrapNodeWidget(UEdGraphNode *Node, TSharedRef<SGraphNode> NodeWidget, int NodeIndex) const
{
    TSharedRef<SWidget> WidgetToWrap = NodeWidget->GetSlot(ENodeZone::Center)->GetWidget();
    check(WidgetToWrap != SNullWidget::NullWidget);

    NodeWidget->GetOrAddSlot(ENodeZone::Center)
        .HAlign(HAlign_Fill)
            [SNew(SVerticalBox)

             + SVerticalBox::Slot()
                   .HAlign(HAlign_Fill)
                   .AutoHeight()
                   .Padding(FMargin(1.5f, 0.25f))
                       [SNew(SOverlay)

                        + SOverlay::Slot()
                              [SNew(SImage)
                                   .Image(FAppStyle::Get().GetBrush("Graph.Node.Body"))]

                        + SOverlay::Slot()
                              .Padding(FMargin(4.0f, 0.0f))
                                  [SNew(SHorizontalBox) + SHorizontalBox::Slot()
                                                              .HAlign(HAlign_Right)
                                                              .VAlign(VAlign_Center)
                                                              .Padding(1.f)
                                                                  [SNew(SOverlay) + SOverlay::Slot()
                                                                                        [SNew(SIndexer)
                                                                                             .IndexValue(NodeIndex)
                                                                                             .TextColor(FLinearColor::White)
                                                                                             .BorderColor(FLinearColor::Gray)]]]]

             + SVerticalBox::Slot()
                   .HAlign(HAlign_Fill)
                   .AutoHeight()
                       [WidgetToWrap]];
}

void FAccessionNodeFactory::WrapPinWidget(UEdGraphPin *Pin, TSharedRef<SGraphPin> PinWidget, int PinIndex, SGraphNode *OwnerNode) const
{
    TSharedRef<SWidget> PinWidgetContent = PinWidget->GetContent();
    check(PinWidgetContent != SNullWidget::NullWidget);

    TSharedRef<SWidget> IndexOverlay = SNew(SOverlay)
                                           .Visibility_Lambda([OwnerNode]() -> EVisibility
                                                              {

            if (OwnerNode->HasAnyUserFocusOrFocusedDescendants() || OwnerNode->IsHovered() || OwnerNode->GetOwnerPanel()->SelectionManager.IsNodeSelected(OwnerNode->GetNodeObj()))
                return EVisibility::Visible;

            return EVisibility::Hidden; }) +
                                       SOverlay::Slot()
                                           [SNew(SIndexer)
                                                .IndexValue(PinIndex)
                                                .TextColor(FLinearColor::White)
                                                .BorderColor(FLinearColor::Gray)];

    switch (Pin->Direction)
    {
    case EEdGraphPinDirection::EGPD_Input:
    {
        PinWidget->SetContent(
            SNew(SHorizontalBox) + SHorizontalBox::Slot().AutoWidth()[PinWidgetContent] + SHorizontalBox::Slot().AutoWidth()[IndexOverlay]);

        break;
    }

    case EEdGraphPinDirection::EGPD_Output:
    {
        PinWidget->SetContent(
            SNew(SHorizontalBox) + SHorizontalBox::Slot().AutoWidth()[IndexOverlay] + SHorizontalBox::Slot().AutoWidth()[PinWidgetContent]);
        break;
    }

    default:
    {
        UE_LOG(LogAccession, Error, TEXT("Pin Direction Not Recognized"));
        break;
    }
    }
}