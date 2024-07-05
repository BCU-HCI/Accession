#include "PhraseEvents/NodeInteractionLibrary.h"
#include "PhraseEvents/Utils.h"

#include "BlueprintEditor.h"
#include "SNodePanel.h"
#include "SGraphPanel.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"

#include "PhraseTree/Containers/Input/InputContainers.h"
#include "AccessibilityWrappers/AccessibilityAddNodeContextMenu.h"
#include "AccessibilityWrappers/AccessibilityGraphLocomotionContext.h"

#include "PhraseTree/PhraseInputNode.h"
#include "PhraseTree/PhraseStringInputNode.h"
#include "PhraseTree/PhraseDirectionalInputNode.h"
#include "PhraseTree/PhraseContextNode.h"
#include "PhraseTree/PhraseContextMenuNode.h"
#include "PhraseTree/PhraseEventNode.h"

UNodeInteractionLibrary::UNodeInteractionLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UNodeInteractionLibrary::~UNodeInteractionLibrary() 
{

}

void UNodeInteractionLibrary::BindBranches(TSharedRef<FPhraseTree> PhraseTree) 
{
	// Events
	TDelegate<void(int32)> NodeIndexFocusDelegate = CreateInputDelegate(this, &UNodeInteractionLibrary::NodeIndexFocus);


	// Add Node Children Branch
	TPhraseNodeArray AddNodeContextChildren = TPhraseNodeArray {

		MakeShared<FPhraseNode>(TEXT("SELECT"), 
		TPhraseNodeArray {

			MakeShared<FPhraseInputNode<int32>>(TEXT("SELECTION_INDEX"),
			TPhraseNodeArray {
				
				MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::NodeAddSelect))

			})

		}),

		MakeShared<FPhraseNode>(TEXT("SEARCH"), 
		TPhraseNodeArray{

			MakeShared<FPhraseNode>(TEXT("ADD"), 
			TPhraseNodeArray {
				
				MakeShared<FPhraseStringInputNode>(TEXT("SEARCH_PHRASE"),
				TPhraseNodeArray{
					
					MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::NodeAddSearchAdd))

				})

			}),

			MakeShared<FPhraseNode>(TEXT("REMOVE"),
			TPhraseNodeArray {

				MakeShared<FPhraseInputNode<int32>>(TEXT("AMOUNT"),
				TPhraseNodeArray {

					MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::NodeAddSearchRemove))

				})

			}),

			MakeShared<FPhraseNode>(TEXT("RESET"), 
			TPhraseNodeArray {

				MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::NodeAddSearchReset))

			})

		}),

		MakeShared<FPhraseNode>(TEXT("SCROLL"), 
		TPhraseNodeArray {
		
			MakeShared<FPhraseScrollInputNode>(TEXT("DIRECTION"), 
			TPhraseNodeArray {

				MakeShared<FPhraseInputNode<int32>>(TEXT("AMOUNT"),
				TPhraseNodeArray {

					MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::NodeAddScroll))

				})

			}),

		}),


		MakeShared<FPhraseNode>(TEXT("CONTEXT"), 
		TPhraseNodeArray {
		
			MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::NodeAddToggleContext))

		})

	};

	PhraseTree->BindBranches(
		TPhraseNodeArray
		{
			MakeShared<FPhraseNode>(TEXT("NODE"),
			TPhraseNodeArray {

				MakeShared<FPhraseInputNode<int32>>(TEXT("NODE_INDEX"),
				TPhraseNodeArray {

					MakeShared<FPhraseNode>(TEXT("MOVE"),
					TPhraseNodeArray {

						MakeShared<FPhrase2DDirectionalInputNode>(TEXT("DIRECTION"),
						TPhraseNodeArray {

							MakeShared<FPhraseInputNode<int32>>(TEXT("AMOUNT"),
							TPhraseNodeArray {

								MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::MoveNode))

							})

						})

					}),

					MakeShared<FPhraseNode>(TEXT("REMOVE"),
					TPhraseNodeArray {

						MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::DeleteNode))

					}),

					MakeShared<FPhraseNode>(TEXT("PIN"),
					TPhraseNodeArray {

						MakeShared<FPhraseInputNode<int32>>(TEXT("PIN_INDEX"),
						TPhraseNodeArray {

							MakeShared<FPhraseNode>(TEXT("CONNECT"),
							TPhraseNodeArray {

								MakeShared<FPhraseNode>(TEXT("NODE"),
								TPhraseNodeArray {

									MakeShared<FPhraseContextMenuNode<UAccessibilityAddNodeContextMenu>>(
										TEXT("ADD"),
										1.5f,
										CreateMenuDelegate(this, &UNodeInteractionLibrary::NodeAddPinMenu),
										AddNodeContextChildren
									),

									MakeShared<FPhraseInputNode<int32>>(TEXT("NODE_INDEX"),
									TPhraseNodeArray {

										MakeShared<FPhraseNode>(TEXT("PIN"),
										TPhraseNodeArray {

											MakeShared<FPhraseInputNode<int32>>(TEXT("PIN_INDEX"),
											TPhraseNodeArray {

												MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::PinConnect))

											})

										})

									}, NodeIndexFocusDelegate)

								})

							}),

							MakeShared<FPhraseNode>(TEXT("DISCONNECT"),
							TPhraseNodeArray {

								MakeShared<FPhraseNode>(TEXT("NODE"),
								TPhraseNodeArray {

									MakeShared<FPhraseInputNode<int32>>(TEXT("NODE_INDEX"),
									TPhraseNodeArray {


										MakeShared<FPhraseNode>(TEXT("PIN"),
										TPhraseNodeArray {

											MakeShared<FPhraseInputNode<int32>>(TEXT("PIN_INDEX"),
											TPhraseNodeArray {

												MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::PinDisconnect))

											})

										})

									}, NodeIndexFocusDelegate)

								})

							})

						})

					})

				}, NodeIndexFocusDelegate),

				MakeShared<FPhraseNode>(TEXT("SELECT"),
				TPhraseNodeArray {

					MakeShared<FPhraseInputNode<int32>>(TEXT("NODE_INDEX"),
					TPhraseNodeArray {

						MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::SelectionNodeToggle))

					}),

					MakeShared<FPhraseNode>(TEXT("RESET"),
					TPhraseNodeArray {

						MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::SelectionReset))

					}),

					MakeShared<FPhraseNode>(TEXT("MOVE"),
					TPhraseNodeArray {

						MakeShared<FPhrase2DDirectionalInputNode>(TEXT("DIRECTION"),
						TPhraseNodeArray {

							MakeShared<FPhraseInputNode<int32>>(TEXT("AMOUNT"),
							TPhraseNodeArray {

								MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::SelectionMove))

							})

						})

					}),

					MakeShared<FPhraseNode>(TEXT("STRAIGHTEN"),
					TPhraseNodeArray {

						MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::SelectionStraighten))

					}),

					MakeShared<FPhraseNode>(TEXT("ALIGNMENT"),
					TPhraseNodeArray {

						MakeShared<FPhrasePositionalInputNode>(TEXT("POSITION"),
						TPhraseNodeArray {

							MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::SelectionAlignment))

						})

					}),

					MakeShared<FPhraseNode>(TEXT("COMMENT"),
					TPhraseNodeArray{

						MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::SelectionComment))

					})

				}),

				MakeShared<FPhraseContextMenuNode<UAccessibilityAddNodeContextMenu>>(
					TEXT("ADD"),
					1.5f,
					CreateMenuDelegate(this, &UNodeInteractionLibrary::NodeAddMenu),
					AddNodeContextChildren
				),

			}),

			MakeShared<FPhraseNode>(TEXT("GRAPH"),
			TPhraseNodeArray {

				MakeShared<FPhraseNode>(TEXT("COMPILE"),
				TPhraseNodeArray {

					MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::BlueprintCompile))

				}),

				MakeShared<FPhraseContextNode<UAccessibilityGraphLocomotionContext>>(TEXT("MOVE"),
				TPhraseNodeArray {

					MakeShared<FPhraseNode>(TEXT("SELECT"),
					TPhraseNodeArray {

						MakeShared<FPhraseInputNode<int32>>(TEXT("INDEX"),
						TPhraseNodeArray {

							MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::LocomotionSelect))

						})

					}),

					MakeShared<FPhraseNode>(TEXT("REVERT"),
					TPhraseNodeArray {

						MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::LocomotionRevert))

					}),

					MakeShared<FPhraseNode>(TEXT("CONFIRM"),
					TPhraseNodeArray {

						MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::LocomotionConfirm))

					}),

				}),
			})
		}
	);

};


void UNodeInteractionLibrary::MoveNode(FParseRecord &Record) {
	GET_ACTIVE_TAB(ActiveGraphEditor, SGraphEditor)

	UParseIntInput* IndexInput = Record.GetPhraseInput<UParseIntInput>(TEXT("NODE_INDEX"));
	UParseEnumInput* DirectionInput = Record.GetPhraseInput<UParseEnumInput>(TEXT("DIRECTION"));
	UParseIntInput* AmountInput = Record.GetPhraseInput<UParseIntInput>(TEXT("AMOUNT"));
	if (IndexInput == nullptr || DirectionInput == nullptr || AmountInput == nullptr)
		return;

	TSharedRef<FAssetAccessibilityRegistry> AssetRegistry = GetAssetRegistry();
	TSharedRef<FGraphIndexer> Indexer = AssetRegistry->GetGraphIndexer(ActiveGraphEditor->GetCurrentGraph());

	UEdGraphNode* Node = Indexer->GetNode(IndexInput->GetValue());
	if (Node == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("MoveNode: Node Not Found"));
		return;
	}

	FVector2D PositionDelta = FVector2D::ZeroVector;
	switch (EPhrase2DDirectionalInput(DirectionInput->GetValue()))
	{
		case EPhrase2DDirectionalInput::UP:
            PositionDelta.Y -= AmountInput->GetValue();
			break;

		case EPhrase2DDirectionalInput::DOWN:
			PositionDelta.Y += AmountInput->GetValue();
			break;

		case EPhrase2DDirectionalInput::LEFT:
			PositionDelta.X -= AmountInput->GetValue();
			break;

		case EPhrase2DDirectionalInput::RIGHT:
			PositionDelta.X += AmountInput->GetValue();
			break;

		default:
			UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("MoveNode: Invalid Direction"));
			return;
	}

	SGraphPanel* GraphPanel = ActiveGraphEditor->GetGraphPanel();
	if (GraphPanel == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("MoveNode: Linked Graph Panel Not Found"));
	}

	TSharedPtr<SGraphNode> NodeWidget = GraphPanel ? GraphPanel->GetNodeWidgetFromGuid(Node->NodeGuid) : TSharedPtr<SGraphNode>();
	if (NodeWidget.IsValid())
	{
        SNodePanel::SNode::FNodeSet NodeFilter;
		NodeWidget->MoveTo(FVector2D(Node->NodePosX, Node->NodePosY) + PositionDelta, NodeFilter);
	}
	else 
	{
		Node->Modify();
		Node->NodePosX += PositionDelta.X;
		Node->NodePosY += PositionDelta.Y;
	}

	// Move Comment Node Children
    // Note: This is a workaround for the MoveTo Function not calling the override in UEdGraphNode_Comment
	if (UEdGraphNode_Comment* CommentNode = Cast<UEdGraphNode_Comment>(Node))
	{
		for (UObject* _CommentChildNode : CommentNode->GetNodesUnderComment())
		{
			if (UEdGraphNode* CommentChildNode = Cast<UEdGraphNode>(_CommentChildNode))
			{
				if (!GraphPanel->SelectionManager.IsNodeSelected(CommentChildNode))
				{
					CommentChildNode->Modify();
					CommentChildNode->NodePosX += PositionDelta.X;
					CommentChildNode->NodePosY += PositionDelta.Y;
				}
			}
		}
	}
}

void UNodeInteractionLibrary::DeleteNode(FParseRecord& Record)
{
	GET_ACTIVE_TAB(ActiveGraphEditor, SGraphEditor)

	UParseIntInput* IndexInput = Record.GetPhraseInput<UParseIntInput>(TEXT("NODE_INDEX"));
	if (IndexInput == nullptr)
		return;

	TSharedRef<FAssetAccessibilityRegistry> AssetRegistry = GetAssetRegistry();
    TSharedRef<FGraphIndexer> Indexer = AssetRegistry->GetGraphIndexer(ActiveGraphEditor->GetCurrentGraph());

	UEdGraphNode* Node = Indexer->GetNode(IndexInput->GetValue());
	if (Node == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("DeleteNode: Node Not Found"));
		return;
	}

	Node->Modify();
	Node->DestroyNode();
}

void UNodeInteractionLibrary::NodeIndexFocus(int32 Index)
{
	GET_ACTIVE_TAB(ActiveGraphEditor, SGraphEditor)

	TSharedRef<FGraphIndexer> Indexer = GetAssetRegistry()->GetGraphIndexer(
		ActiveGraphEditor->GetCurrentGraph()
	);

	UEdGraphNode* Node = Indexer->GetNode(Index);
	if (Node == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("NodeSelectionFocus: Node Not Found"));
		return;
	}

	ActiveGraphEditor->ClearSelectionSet();
	ActiveGraphEditor->SetNodeSelection(Node, true);
}

void UNodeInteractionLibrary::PinConnect(FParseRecord& Record) 
{
	GET_ACTIVE_TAB(ActiveGraphEditor, SGraphEditor)

	UEdGraph* Graph = ActiveGraphEditor->GetCurrentGraph();

	TArray<UParseInput*> NodeInputs = Record.GetPhraseInputs(TEXT("NODE_INDEX"));
	TArray<UParseInput*> PinInputs = Record.GetPhraseInputs(TEXT("PIN_INDEX"));

	if (NodeInputs.Num() != 2 || PinInputs.Num() != 2)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("PinConnect: Invalid Inputs Amount"));
		return;
	}

	TSharedRef<FGraphIndexer> Indexer = GetAssetRegistry()->GetGraphIndexer(Graph);

	UEdGraphPin* SourcePin = Indexer->GetPin(
		Cast<UParseIntInput>(NodeInputs[0])->GetValue(), 
		Cast<UParseIntInput>(PinInputs[0])->GetValue()
	);

	UEdGraphPin* TargetPin = Indexer->GetPin(
		Cast<UParseIntInput>(NodeInputs[1])->GetValue(),
		Cast<UParseIntInput>(PinInputs[1])->GetValue()
	);

	if (SourcePin == nullptr || TargetPin == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("PinConnect: Pins Not Found"));
		return;
	}

	if (!Graph->GetSchema()->TryCreateConnection(SourcePin, TargetPin))
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("PinConnect: Pin Connection Failed"));
	}
}

void UNodeInteractionLibrary::PinDisconnect(FParseRecord& Record) 
{
	GET_ACTIVE_TAB(ActiveGraphEditor, SGraphEditor)

	UEdGraph* Graph = ActiveGraphEditor->GetCurrentGraph();

	TArray<UParseInput*> NodeInputs = Record.GetPhraseInputs(TEXT("NODE_INDEX"));
	TArray<UParseInput*> PinInputs = Record.GetPhraseInputs(TEXT("PIN_INDEX"));

	if (NodeInputs.Num() != 2 || PinInputs.Num() != 2)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("PinDisconnect: Invalid Inputs Amount"));
		return;
	}

	TSharedRef<FGraphIndexer> Indexer = GetAssetRegistry()->GetGraphIndexer(Graph);

	UEdGraphPin* SourcePin = Indexer->GetPin(
		Cast<UParseIntInput>(NodeInputs[0])->GetValue(),
		Cast<UParseIntInput>(PinInputs[0])->GetValue()
	);

	UEdGraphPin* TargetPin = Indexer->GetPin(
		Cast<UParseIntInput>(NodeInputs[1])->GetValue(),
		Cast<UParseIntInput>(PinInputs[1])->GetValue()
	);

	if (SourcePin == nullptr || TargetPin == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("PinDisconnect: Pins Not Found"));
		return;
	}

	Graph->GetSchema()->BreakSinglePinLink(SourcePin, TargetPin);
}

TSharedPtr<IMenu> UNodeInteractionLibrary::NodeAddMenu(FParseRecord& Record) 
{
	GET_ACTIVE_TAB_RETURN(ActiveGraphEditor, SGraphEditor, TSharedPtr<IMenu>())

	SGraphPanel* GraphPanel = ActiveGraphEditor->GetGraphPanel();
    
	FVector2D SpawnLocation;
	{
		TSharedPtr<SWindow> TopLevelWindow = FSlateApplication::Get().GetActiveTopLevelRegularWindow();

		if (TopLevelWindow.IsValid())
		{
			SpawnLocation = TopLevelWindow->GetPositionInScreen();
			FVector2D WindowSize = TopLevelWindow->GetSizeInScreen();

			SpawnLocation.X += WindowSize.X / 5;
            SpawnLocation.Y += WindowSize.Y / 5;
		}
		else
		{
            FDisplayMetrics DisplayMetrics;
            FSlateApplication::Get().GetDisplayMetrics(DisplayMetrics);

			SpawnLocation = FVector2D(
				DisplayMetrics.PrimaryDisplayWidth / 5,
				DisplayMetrics.PrimaryDisplayHeight / 5
			);
		}

		TSharedPtr<SWidget> ContextWidgetToFocus = GraphPanel->SummonContextMenu(
            SpawnLocation, 
			GraphPanel->GetPastePosition(),
			nullptr,
			nullptr, 
			TArray<UEdGraphPin *>()
		);

		if (!ContextWidgetToFocus.IsValid())
		{
			UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("NodeAddMenu: Context Keyboard Focus Widget Not Found"));            
			return TSharedPtr<IMenu>();
		}

		FSlateApplication::Get().SetKeyboardFocus(ContextWidgetToFocus);

		FWidgetPath KeyboardFocusPath;
		if (FSlateApplication::Get().FindPathToWidget(ContextWidgetToFocus.ToSharedRef(), KeyboardFocusPath))
		{
			return FSlateApplication::Get().FindMenuInWidgetPath(KeyboardFocusPath);
		} 
		else 
		{
			UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("NodeAddMenu: IMenu Could Not Be Found In Widget Path"))
			return TSharedPtr<IMenu>();
		}
	}
}

TSharedPtr<IMenu> UNodeInteractionLibrary::NodeAddPinMenu(FParseRecord &Record) 
{
	GET_ACTIVE_TAB_RETURN(ActiveGraphEditor, SGraphEditor, TSharedPtr<IMenu>())

	SGraphPanel* GraphPanel = ActiveGraphEditor->GetGraphPanel();
    
	FVector2D SpawnLocation;
	{
		TSharedPtr<SWindow> TopLevelWindow = FSlateApplication::Get().GetActiveTopLevelRegularWindow();

		if (TopLevelWindow.IsValid())
		{
			SpawnLocation = TopLevelWindow->GetPositionInScreen();
			FVector2D WindowSize = TopLevelWindow->GetSizeInScreen();

			SpawnLocation.X += WindowSize.X / 5;
            SpawnLocation.Y += WindowSize.Y / 5;
		}
		else
		{
            FDisplayMetrics DisplayMetrics;
            FSlateApplication::Get().GetDisplayMetrics(DisplayMetrics);

			SpawnLocation = FVector2D(
				DisplayMetrics.PrimaryDisplayWidth / 5,
				DisplayMetrics.PrimaryDisplayHeight / 5
			);
		}

		TSharedRef<FGraphIndexer> Indexer = GetAssetRegistry()->GetGraphIndexer(ActiveGraphEditor->GetCurrentGraph());

		UParseIntInput* NodeIndexInput = Record.GetPhraseInput<UParseIntInput>(TEXT("NODE_INDEX"));
		UParseIntInput* PinIndexInput = Record.GetPhraseInput<UParseIntInput>(TEXT("PIN_INDEX"));

		if (NodeIndexInput == nullptr || PinIndexInput == nullptr)
		{
			UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("NodeAddMenu: Invalid Inputs"));
			
			return TSharedPtr<IMenu>();
		}

		TSharedPtr<SWidget> ContextWidgetToFocus = GraphPanel->SummonContextMenu(
            SpawnLocation, 
			GraphPanel->GetPastePosition(),
			nullptr,
			nullptr, 
			TArray<UEdGraphPin*> {
				Indexer->GetPin(
					NodeIndexInput->GetValue(),
					PinIndexInput->GetValue()
				)
			}
		);

		if (!ContextWidgetToFocus.IsValid())
		{
			UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("NodeAddMenu: Context Keyboard Focus Widget Not Found"));            
			return TSharedPtr<IMenu>();
		}

		FSlateApplication::Get().SetKeyboardFocus(ContextWidgetToFocus);

		FWidgetPath KeyboardFocusPath;
		if (FSlateApplication::Get().FindPathToWidget(ContextWidgetToFocus.ToSharedRef(), KeyboardFocusPath))
		{
			return FSlateApplication::Get().FindMenuInWidgetPath(KeyboardFocusPath);
		} 
		else 
		{
			UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("NodeAddMenu: IMenu Could Not Be Found In Widget Path"))
			return TSharedPtr<IMenu>();
		}
	}
}

void UNodeInteractionLibrary::NodeAddSelect(FParseRecord& Record) 
{
	GET_TOP_CONTEXT(Record, ContextMenu, UAccessibilityAddNodeContextMenu)

	UParseIntInput* IndexInput = Record.GetPhraseInput<UParseIntInput>(TEXT("SELECTION_INDEX"));
	if (IndexInput == nullptr)
		return;

	ContextMenu->PerformGraphAction(IndexInput->GetValue());
}

void UNodeInteractionLibrary::NodeAddSearchAdd(FParseRecord& Record)
{
	GET_TOP_CONTEXT(Record, ContextMenu, UAccessibilityAddNodeContextMenu)

	UParseStringInput *SearchInput = Record.GetPhraseInput<UParseStringInput>(TEXT("SEARCH_PHRASE"));
	if (SearchInput == nullptr)
		return;

	ContextMenu->AppendFilterText(SearchInput->GetValue());
}

void UNodeInteractionLibrary::NodeAddSearchRemove(FParseRecord& Record)
{
	GET_TOP_CONTEXT(Record, ContextMenu, UAccessibilityAddNodeContextMenu);

	UParseIntInput* RemoveAmountInput = Record.GetPhraseInput<UParseIntInput>(TEXT("AMOUNT"));
	if (RemoveAmountInput == nullptr)
		return;

	ContextMenu->SetFilterText(
		EventUtils::RemoveWordsFromEnd(ContextMenu->GetFilterText(), RemoveAmountInput->GetValue())
	);
}

void UNodeInteractionLibrary::NodeAddSearchReset(FParseRecord& Record)
{
	GET_TOP_CONTEXT(Record, ContextMenu, UAccessibilityAddNodeContextMenu)

	ContextMenu->ResetFilterText();
}

void UNodeInteractionLibrary::NodeAddScroll(FParseRecord& Record)
{
	GET_TOP_CONTEXT(Record, ContextMenu, UAccessibilityAddNodeContextMenu)

	UParseEnumInput* DirectionInput = Record.GetPhraseInput<UParseEnumInput>(TEXT("DIRECTION"));
	UParseIntInput* AmountInput = Record.GetPhraseInput<UParseIntInput>(TEXT("AMOUNT"));
	if (DirectionInput == nullptr || AmountInput == nullptr)
		return;

	switch (EPhraseScrollInput(DirectionInput->GetValue()))
	{
        case EPhraseScrollInput::UP:
			ContextMenu->AppendScrollDistance(-AmountInput->GetValue());
            break;

		case EPhraseScrollInput::DOWN:
            ContextMenu->AppendScrollDistance(AmountInput->GetValue());
            break;

		case EPhraseScrollInput::TOP:
            ContextMenu->SetScrollDistanceTop();
            break;

		case EPhraseScrollInput::BOTTOM:
			ContextMenu->SetScrollDistanceBottom();
				break;

		default:
			UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("NodeAddScroll: Invalid Scroll Position / Direction"));
			return;
	}
}

void UNodeInteractionLibrary::NodeAddToggleContext(FParseRecord& Record)
{
	GET_TOP_CONTEXT(Record, ContextMenu, UAccessibilityAddNodeContextMenu)

	ContextMenu->ToggleContextAwareness();
}

void UNodeInteractionLibrary::SelectionNodeToggle(FParseRecord& Record)
{
	GET_ACTIVE_TAB(ActiveGraphEditor, SGraphEditor);

	UParseIntInput* IndexInput = Record.GetPhraseInput<UParseIntInput>(TEXT("NODE_INDEX"));
	if (IndexInput == nullptr)
		return;

	TSharedRef<FGraphIndexer> Indexer = GetAssetRegistry()->GetGraphIndexer(
		ActiveGraphEditor->GetCurrentGraph()
	);

	UEdGraphNode* Node = Indexer->GetNode(IndexInput->GetValue());
	if (Node == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("SelectionToggle: Node Not Found"));
		return;
	}

	ActiveGraphEditor->SetNodeSelection(
		Node, 
		!ActiveGraphEditor->GetSelectedNodes().Contains(Node)
	);
}

void UNodeInteractionLibrary::SelectionReset(FParseRecord &Record) {
	GET_ACTIVE_TAB(ActiveGraphEditor, SGraphEditor)

	ActiveGraphEditor->ClearSelectionSet();
}

void UNodeInteractionLibrary::SelectionMove(FParseRecord& Record)
{
	GET_ACTIVE_TAB(ActiveGraphEditor, SGraphEditor)

	UParseEnumInput* Direction = Record.GetPhraseInput<UParseEnumInput>(TEXT("DIRECTION"));
	UParseIntInput* Amount = Record.GetPhraseInput<UParseIntInput>(TEXT("AMOUNT"));
	if (Direction == nullptr || Amount == nullptr)
		return;

	for (UObject* NodeObj : ActiveGraphEditor->GetSelectedNodes())
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(NodeObj);
		if (Node == nullptr)
			continue;

		switch (EPhrase2DDirectionalInput(Direction->GetValue()))
		{
			case EPhrase2DDirectionalInput::UP:
				Node->NodePosY -= Amount->GetValue();
				break;

			case EPhrase2DDirectionalInput::DOWN:
				Node->NodePosY += Amount->GetValue();
				break;

			case EPhrase2DDirectionalInput::LEFT:
				Node->NodePosX -= Amount->GetValue();
				break;

			case EPhrase2DDirectionalInput::RIGHT:
				Node->NodePosX += Amount->GetValue();
				break;

			default:
				UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("SelectionMove: Invalid Direction"));
				return;
		}
	}
}

void UNodeInteractionLibrary::SelectionAlignment(FParseRecord& Record)
{
	GET_ACTIVE_TAB(ActiveGraphEditor, SGraphEditor)

	UParseEnumInput* PositionInput = Record.GetPhraseInput<UParseEnumInput>(TEXT("POSITION"));
	if (PositionInput == nullptr)
		return;

	switch (EPhrasePositionalInput(PositionInput->GetValue()))
	{
		case EPhrasePositionalInput::TOP:
			ActiveGraphEditor->OnAlignTop();
			break;

		case EPhrasePositionalInput::MIDDLE:
			ActiveGraphEditor->OnAlignMiddle();
			break;

		case EPhrasePositionalInput::BOTTOM:
			ActiveGraphEditor->OnAlignBottom();
			break;

		case EPhrasePositionalInput::LEFT:
			ActiveGraphEditor->OnAlignLeft();
			break;

		case EPhrasePositionalInput::RIGHT:
			ActiveGraphEditor->OnAlignRight();
			break;

		case EPhrasePositionalInput::CENTER:
			ActiveGraphEditor->OnAlignCenter();
			break;
	}
}

void UNodeInteractionLibrary::SelectionStraighten(FParseRecord& Record)
{
	GET_ACTIVE_TAB(ActiveGraphEditor, SGraphEditor)

	ActiveGraphEditor->OnStraightenConnections();
}

void UNodeInteractionLibrary::SelectionComment(FParseRecord& Record)
{
	GET_ACTIVE_TAB(ActiveGraphEditor, SGraphEditor)

	UEdGraph* Graph = ActiveGraphEditor->GetCurrentGraph();
	
	TSharedPtr<FEdGraphSchemaAction> CommentCreateAction = Graph->GetSchema()->GetCreateCommentAction();
    if (CommentCreateAction.IsValid()) 
	{
		CommentCreateAction->PerformAction(Graph, nullptr, FVector2D(0, 0), true);
    }
	else UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("SelectionComment: Comment Creation Failed"));
}

void UNodeInteractionLibrary::LocomotionSelect(FParseRecord& Record)
{
	GET_TOP_CONTEXT(Record, LocomotionContext, UAccessibilityGraphLocomotionContext);

	UParseIntInput* ViewSelectionInput = Record.GetPhraseInput<UParseIntInput>(TEXT("INDEX"));
	if (ViewSelectionInput == nullptr)
		return;

	if (!LocomotionContext->SelectChunk(ViewSelectionInput->GetValue()))
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("Locomotion Select: Failed to Choose New View."));
	}
}

void UNodeInteractionLibrary::LocomotionRevert(FParseRecord& Record)
{
	GET_TOP_CONTEXT(Record, LocomotionContext, UAccessibilityGraphLocomotionContext);

	if (!LocomotionContext->RevertToPreviousView())
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("Locomotion Revert: Failed to Revert to Previous View."));
	}
}

void UNodeInteractionLibrary::LocomotionConfirm(FParseRecord& Record)
{
	GET_TOP_CONTEXT(Record, LocomotionContext, UAccessibilityGraphLocomotionContext);

	LocomotionContext->ConfirmSelection(); 
}

void UNodeInteractionLibrary::BlueprintCompile(FParseRecord& Record)
{
    GET_ACTIVE_TAB(ActiveGraphEditor, SGraphEditor)

    UEdGraph* ActiveGraph = ActiveGraphEditor->GetCurrentGraph();
	if (ActiveGraph == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("BlueprintCompile: Active Graph Not Found"));
		return;
	}

	UBlueprint* FoundBlueprint = FBlueprintEditorUtils::FindBlueprintForGraph(ActiveGraph);
	if (FoundBlueprint == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("BlueprintCompile: Blueprint Not Found"));
		return;
	}

	TSharedPtr<FBlueprintEditor> BlueprintEditor = StaticCastSharedPtr<FBlueprintEditor>(FKismetEditorUtilities::GetIBlueprintEditorForObject(FoundBlueprint, false));
	if (!BlueprintEditor.IsValid())
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("BlueprintCompile: BlueprintEditor Not Found"));
		return;
	}

	BlueprintEditor->Compile();
}
