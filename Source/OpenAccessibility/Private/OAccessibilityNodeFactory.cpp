// Fill out your copyright notice in the Description page of Project Settings.


#include "OAccessibilityNodeFactory.h"
#include "OpenAccessibilityLogging.h"

#include "Logging/StructuredLog.h"

#include "NodeFactory.h"
#include "EdGraphUtilities.h"

#include "SNodePanel.h"
#include "SGraphNode.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

/*
#include "K2Node.h"
#include "K2Node_AddPinInterface.h"
#include "K2Node_CallMaterialParameterCollectionFunction.h"
#include "K2Node_Composite.h"
#include "K2Node_Copy.h"
#include "K2Node_CreateDelegate.h"
#include "K2Node_Event.h"
#include "K2Node_FormatText.h"
#include "K2Node_Knot.h"
#include "K2Node_MakeStruct.h"
#include "K2Node_PromotableOperator.h"
#include "K2Node_SpawnActor.h"
#include "K2Node_SpawnActorFromClass.h"
#include "K2Node_Switch.h"
#include "K2Node_Timeline.h"
#include "SGraphNodePromotableOperator.h"
#include "SGraphNodeKnot.h"
#include "SGraphNodeDocumentation.h"
#include "KismetNodes/SGraphNodeK2Composite.h"
#include "KismetNodes/SGraphNodeK2Default.h"
#include "KismetNodes/SGraphNodeK2Event.h"
#include "KismetNodes/SGraphNodeK2Var.h"
#include "EdGraph/EdGraphNode_Documentation.h"
#include "EdGraphNode_Comment.h"
// Private Kismet Node Includes
#include <Editor\GraphEditor\Private\KismetNodes\SGraphNodeMakeStruct.h>
#include <Editor\GraphEditor\Private\KismetNodes\SGraphNodeK2Copy.h>
#include <Editor\GraphEditor\Private\KismetNodes\SGraphNodeSwitchStatement.h>
#include <Editor\GraphEditor\Private\KismetNodes\SGraphNodeK2Sequence.h>
#include <Editor\GraphEditor\Private\KismetNodes\SGraphNodeK2Timeline.h>
#include <Editor\GraphEditor\Private\KismetNodes\SGraphNodeSpawnActor.h>
#include <Editor\GraphEditor\Private\KismetNodes\SGraphNodeSpawnActorFromClass.h>
#include <Editor\GraphEditor\Private\KismetNodes\SGraphNodeK2CreateDelegate.h>
#include <Editor\GraphEditor\Private\KismetNodes\SGraphNodeCallParameterCollectionFunction.h>
#include <Editor\GraphEditor\Private\KismetNodes\SGraphNodeFormatText.h>


#include "MaterialGraph/MaterialGraphNode.h"
#include "MaterialGraph/MaterialGraphNode_Base.h"
#include "MaterialGraph/MaterialGraphNode_Comment.h"
#include "MaterialGraph/MaterialGraphNode_Composite.h"
#include "MaterialGraph/MaterialGraphNode_Root.h"
#include "MaterialGraph/MaterialGraphSchema.h"
#include "MaterialGraphNode_Knot.h"
// Private Material Node Includes

#include <Editor\GraphEditor\Private\MaterialNodes\SGraphNodeMaterialBase.h>
#include <Editor\GraphEditor\Private\MaterialNodes\SGraphNodeMaterialResult.h>
#include <Editor\GraphEditor\Private\MaterialNodes\SGraphNodeMaterialComment.h>

#include "KismetNodes\SGraphNodeK2Composite.h"
*/

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

    // Build Accessibility Widget. - Can Be Replaced with a Custom GraphNode Class
	TSharedPtr<SWidget> NodeNumBlock = SNullWidget::NullWidget;
    SAssignNew(NodeNumBlock, SBorder)
        .HAlign( HAlign_Right )
        .VAlign( VAlign_Top )
        .Padding( 0.5f )
        [
            SNew(SVerticalBox)
                + SVerticalBox::Slot()
				.AutoHeight()
				.HAlign( HAlign_Fill )
				.Padding( 0.5f )
                [
                    SNew(STextBlock)
                        .Text( FText::FromString("[" + FString::FromInt(NodeIndex) + "]") )
				]

                /*
                + SVerticalBox::Slot()
                .HAlign( HAlign_Fill )
                .AutoHeight()
                [
                    OutNode.ToSharedRef()
                ]
                */
        ];

    // Bind Accessibility Widget to OutNode. - Can Be Replaced with a Custom GraphNode Class, 
    // that passes the Original Node to the Accessibility Node.
    OutNode->GetOrAddSlot(ENodeZone::TopRight)
        .SlotOffset_Lambda([NodeNumBlock, OutNode] { return FVector2D(OutNode->GetDesiredSize().X - NodeNumBlock->GetDesiredSize().X, -NodeNumBlock->GetDesiredSize().Y); })
        .SlotSize_Lambda([NodeNumBlock] { return NodeNumBlock->GetDesiredSize(); })
        .VAlign(VAlign_Top)
        [
			NodeNumBlock.ToSharedRef()
		];

    return OutNode;
}