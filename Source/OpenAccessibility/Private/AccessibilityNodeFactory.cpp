// Copyright F-Dudley. All Rights Reserved.


#include "AccessibilityNodeFactory.h"
#include "OpenAccessibility.h"

#include "SGraphNode.h"
#include "SGraphPin.h"

FGraphAccessibilityNodeFactory::FGraphAccessibilityNodeFactory()
{
	AccessibilityRegistry = FOpenAccessibilityModule::Get().AssetAccessibilityRegistry.ToSharedRef();
}

FGraphAccessibilityNodeFactory::FGraphAccessibilityNodeFactory(TSharedRef<FAssetAccessibilityRegistry> InAccessibilityRegistry) : FGraphNodeFactory()
{
	AccessibilityRegistry = InAccessibilityRegistry;
}

FGraphAccessibilityNodeFactory::~FGraphAccessibilityNodeFactory()
{

}

TSharedPtr<class SGraphNode> FGraphAccessibilityNodeFactory::CreateNodeWidget(UEdGraphNode* InNode)
{
	check(InNode != nullptr);

	TSharedPtr<SGraphNode> OutNode = FGraphNodeFactory::CreateNodeWidget(InNode);

	// Apply Accessibility Visuals to the Node.

	TSharedRef<FGraphIndexer> GraphIndexer = AccessibilityRegistry->GetGraphIndexer(InNode->GetGraph());

	int NodeIndex = -1;
	GraphIndexer->GetOrAddNode(InNode);

	TSharedRef<SWidget> WidgetToWrap = OutNode->GetSlot(ENodeZone::Center)->GetWidget();

	check(WidgetToWrap != SNullWidget::NullWidget);

	OutNode->GetOrAddSlot(ENodeZone::Center)
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

	return OutNode;
}

TSharedPtr<class SGraphPin> FGraphAccessibilityNodeFactory::CreatePinWidget(UEdGraphPin* InPin)
{
	check(InPin != nullptr);

	TSharedPtr<SGraphPin> OutPin = FGraphNodeFactory::CreatePinWidget(InPin);
    SGraphPin* OutPinPtr = OutPin.Get();

	TSharedRef<FGraphIndexer> GraphIndexer = AccessibilityRegistry->GetGraphIndexer(InPin->GetOwningNode()->GetGraph());

	int PinIndex = -1;
	PinIndex = InPin->GetOwningNode()->GetPinIndex(InPin);

    TSharedRef<SWidget> AccessiblityWidget = SNew(SOverlay)
        .Visibility_Lambda([OutPinPtr]() -> EVisibility {
            if (OutPinPtr->HasAnyUserFocusOrFocusedDescendants() || OutPinPtr->IsHovered())
                return EVisibility::Visible;

            return EVisibility::Collapsed;
        })
        + SOverlay::Slot()
        [
            SNew(STextBlock)
                .ColorAndOpacity(FLinearColor::White)
                .ShadowColorAndOpacity(FLinearColor::Black)
                .ShadowOffset(FIntPoint(-1, 1))
                .Font(FAppStyle::Get().GetFontStyle("Graph.Node.Pin.Font"))
                .Text(FText::FromString("[" + FString::FromInt(PinIndex) + "]"))
        ];

        // Get Pin Widget Content, before modifying it.
        TSharedRef<SWidget> PinWidgetContent = OutPin->GetContent();

        // Modify the Pin Widget Content, based on the Pin's Direction.
        switch (OutPin->GetDirection())
        {
                case EEdGraphPinDirection::EGPD_Input:
                {
                    OutPin->SetContent(
                        SNew(SHorizontalBox)
                            + SHorizontalBox::Slot()
                            [
                                PinWidgetContent
                            ]
                            + SHorizontalBox::Slot()
                            [
                                AccessiblityWidget
                            ]
                    );

                    break;
                }

                case EEdGraphPinDirection::EGPD_Output:
                {
                    OutPin->SetContent(
                        SNew(SHorizontalBox)
                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            [
                                AccessiblityWidget
                            ]
                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            [
                                PinWidgetContent
                            ]
                    );

                    break;
                }
        }

	return OutPin;
}
