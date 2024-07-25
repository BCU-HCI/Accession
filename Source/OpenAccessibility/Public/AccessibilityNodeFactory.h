// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NodeFactory.h"
#include "OpenAccessibility.h"
#include "AccessibilityWidgets/SIndexer.h"

#include "SGraphNode.h"
#include "SGraphPin.h"

/**
 * 
 */
template<class T>
class OPENACCESSIBILITY_API TGraphAccessibilityNodeFactory : public FGraphNodeFactory
{
public:

	static_assert(TIsDerivedFrom<T, FGraphNodeFactory>::IsDerived, "Provided Template Type Must Derive From FGraphNodeFactory");

	TGraphAccessibilityNodeFactory()
	{
		Implementation = TSharedPtr<T>();

		AccessibilityRegistry = FOpenAccessibilityModule::Get().AssetAccessibilityRegistry.ToSharedRef();
	}

	TGraphAccessibilityNodeFactory(TSharedRef<FAssetAccessibilityRegistry> InAccessibilityRegistry)
		: AccessibilityRegistry(InAccessibilityRegistry)
	{
		Implementation = TSharedPtr<T>();
	}

	virtual ~TGraphAccessibilityNodeFactory()
	{
		
	}

	/* FGraphNodeFactory Implementation */

	/**
	 * Creates a Visual Node Widget from the Provided Node Object.
	 * @param InNode The Node To Create a Node Widget From.
	 * @return 
	 */
	virtual TSharedPtr<class SGraphNode> CreateNodeWidget(UEdGraphNode* InNode) override;

	/**
	 * Creates a Visual Pin Widget from the Provided Pin Object.
	 * @param InPin The Pin to Create a Pin Widget From.
	 * @return 
	 */
	virtual TSharedPtr<class SGraphPin> CreatePinWidget(UEdGraphPin* InPin) override;

	/* End Of FGraphNodeFactory Implementation*/

protected:

	/**
	 * The Asset Registry of the Open Accessibility Plugin.
	 */
	TSharedRef<FAssetAccessibilityRegistry> AccessibilityRegistry;

	TSharedPtr<T> Implementation;
};

template<class T>
TSharedPtr<class SGraphNode> TGraphAccessibilityNodeFactory<T>::CreateNodeWidget(UEdGraphNode* InNode)
{
	check(InNode != nullptr);

	TSharedPtr<SGraphNode> OutNode = Implementation->CreateNodeWidget(InNode);

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
											SNew(SIndexer)
												.IndexValue(NodeIndex)
												.TextColor(FLinearColor::White)
												.BorderColor(FLinearColor::Gray)
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

template<class T>
TSharedPtr<class SGraphPin> TGraphAccessibilityNodeFactory<T>::CreatePinWidget(UEdGraphPin* InPin)
{
	check(InPin != nullptr);

	TSharedPtr<SGraphPin> OutPin = Implementation->CreatePinWidget(InPin);
	SGraphPin* OutPinPtr = OutPin.Get();

	TSharedRef<FGraphIndexer> GraphIndexer = AccessibilityRegistry->GetGraphIndexer(InPin->GetOwningNode()->GetGraph());

	int PinIndex = -1;
	PinIndex = InPin->GetOwningNode()->GetPinIndex(InPin);

	TSharedRef<SWidget> AccessiblityWidget = SNew(SOverlay)
		.Visibility_Lambda([OutPinPtr]() -> EVisibility {
			if (OutPinPtr->HasAnyUserFocusOrFocusedDescendants() || OutPinPtr->IsHovered())
				return EVisibility::Visible;

			return EVisibility::Hidden;
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