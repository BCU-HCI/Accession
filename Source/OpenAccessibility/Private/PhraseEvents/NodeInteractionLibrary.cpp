#include "PhraseEvents/NodeInteractionLibrary.h"
#include "PhraseEvents/Utils.h"

#include "BlueprintEditor.h"
#include "SNodePanel.h"
#include "SGraphPanel.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"

#include "PhraseTree/Containers/Input/InputContainers.h"
#include "AccessibilityWrappers/AccessibilityGraphEditorContext.h"
#include "AccessibilityWrappers/AccessibilityGraphLocomotionContext.h"

#include "PhraseTree/PhraseInputNode.h"
#include "PhraseTree/PhraseStringInputNode.h"
#include "PhraseTree/PhraseDirectionalInputNode.h"
#include "PhraseTree/PhraseContextNode.h"
#include "PhraseTree/PhraseContextMenuNode.h"
#include "PhraseTree/PhraseEventNode.h"

#include "Utils/GraphQuadTree.h"

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

								MakeShared<FPhraseContextMenuNode<UAccessibilityGraphEditorContext>>(
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

									}),

								}, NodeIndexFocusDelegate)

							}),

							MakeShared<FPhraseNode>(TEXT("DISCONNECT"),
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

								})

							}),

							MakeShared<FPhraseNode>(TEXT("SET"), 
							TPhraseNodeArray {

								MakeShared<FPhraseStringInputNode>(TEXT("DEFAULT_VALUE"),
								TPhraseNodeArray {

									MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::PinSetDefault))

								})

							}),

							MakeShared<FPhraseNode>(TEXT("RESET"),
							TPhraseNodeArray {

								MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::PinResetDefault))

							}),

						}),
					}),

					MakeShared<FPhraseNode>(TEXT("RENAME"),
					TPhraseNodeArray {

						MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::RequestRename))

					})

				}, NodeIndexFocusDelegate),

				MakeShared<FPhraseNode>(TEXT("SELECT"),
				TPhraseNodeArray {

					MakeShared<FPhraseInputNode<int32>>(TEXT("NODE_INDEX"),
					TPhraseNodeArray {

						MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::SelectionNodeToggle))

					}),

					MakeShared<FPhraseNode>(TEXT("ALL"),
					TPhraseNodeArray {

						MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::SelectionNodeAll))

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

				MakeShared<FPhraseContextMenuNode<UAccessibilityGraphEditorContext>>(
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

					MakeShared<FPhraseNode>(TEXT("CANCEL"),
					TPhraseNodeArray {

						MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UNodeInteractionLibrary::LocomotionCancel))

					})

				}),
			})
		}
	);

};


void UNodeInteractionLibrary::MoveNode(FParseRecord &Record) {
	GET_CAST_ACTIVE_TAB_CONTENT(ActiveGraphEditor, SGraphEditor)

	// Inputs
	UParseIntInput* IndexInput = Record.GetPhraseInput<UParseIntInput>(TEXT("NODE_INDEX"));
	UParseEnumInput* DirectionInput = Record.GetPhraseInput<UParseEnumInput>(TEXT("DIRECTION"));
	UParseIntInput* AmountInput = Record.GetPhraseInput<UParseIntInput>(TEXT("AMOUNT"));
	if (IndexInput == nullptr || DirectionInput == nullptr || AmountInput == nullptr)
		return;

	// Registry
	TSharedRef<FAssetAccessibilityRegistry> AssetRegistry = GetAssetRegistry();
	TSharedRef<FGraphIndexer> Indexer = AssetRegistry->GetGraphIndexer(ActiveGraphEditor->GetCurrentGraph());

	UEdGraphNode* Node = Indexer->GetNode(IndexInput->GetValue());
	if (Node == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("MoveNode: Node Not Found"));
		return;
	}

	SGraphPanel* GraphPanel = ActiveGraphEditor->GetGraphPanel();

	switch (EPhrase2DDirectionalInput(DirectionInput->GetValue()))
	{
		case EPhrase2DDirectionalInput::UP:
			MoveOnGrid(GraphPanel, Node, FVector2D(0, -AmountInput->GetValue()));
			break;

		case EPhrase2DDirectionalInput::DOWN:
			MoveOnGrid(GraphPanel, Node, FVector2D(0, AmountInput->GetValue()));
			break;

		case EPhrase2DDirectionalInput::LEFT:
			MoveOnGrid(GraphPanel, Node, FVector2D(-AmountInput->GetValue(), 0));
			break;

		case EPhrase2DDirectionalInput::RIGHT:
			MoveOnGrid(GraphPanel, Node, FVector2D(AmountInput->GetValue(), 0));
			break;

		default:
			UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("MoveNode: Invalid Direction"));
		break;
	}
}

void UNodeInteractionLibrary::DeleteNode(FParseRecord& Record)
{
	GET_CAST_ACTIVE_TAB_CONTENT(ActiveGraphEditor, SGraphEditor)

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

void UNodeInteractionLibrary::RequestRename(FParseRecord& Record)
{
	GET_CAST_ACTIVE_TAB_CONTENT(ActiveGraphEditor, SGraphEditor)

	UParseIntInput* IndexInput = Record.GetPhraseInput<UParseIntInput>(TEXT("NODE_INDEX"));
	if (IndexInput == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("RequestRename: Invalid Node Index"));
		return;
	}

	TSharedRef<FGraphIndexer> Indexer = GetAssetRegistry()->GetGraphIndexer(
		ActiveGraphEditor->GetCurrentGraph()
	);

	UEdGraphNode* FoundNode = Indexer->GetNode(IndexInput->GetValue());
	if (FoundNode == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("RequestRename: Node Not Found"));
		return;
	}


	TSharedPtr<SGraphNode> NodeWidget = ActiveGraphEditor->GetGraphPanel()->GetNodeWidgetFromGuid(FoundNode->NodeGuid);
	if (!NodeWidget.IsValid())
	{
		return;
	}

	NodeWidget->RequestRename();
}

void UNodeInteractionLibrary::NodeIndexFocus(int32 Index)
{
	GET_CAST_ACTIVE_TAB_CONTENT(ActiveGraphEditor, SGraphEditor)

	TSharedRef<FGraphIndexer> Indexer = GetAssetRegistry()->GetGraphIndexer(
		ActiveGraphEditor->GetCurrentGraph()
	);

	UEdGraphNode* Node = Indexer->GetNode(Index);
	if (Node == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("NodeSelectionFocus: Node Not Found"));
		return;
	}

	ActiveGraphEditor->SetNodeSelection(Node, true);
}

void UNodeInteractionLibrary::PinConnect(FParseRecord& Record) 
{
	GET_CAST_ACTIVE_TAB_CONTENT(ActiveGraphEditor, SGraphEditor)

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
	GET_CAST_ACTIVE_TAB_CONTENT(ActiveGraphEditor, SGraphEditor)

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

void UNodeInteractionLibrary::PinSetDefault(FParseRecord& Record)
{
	GET_CAST_ACTIVE_TAB_CONTENT(ActiveGraphEditor, SGraphEditor)

	// Get Phrase Inputs
	UParseIntInput* NodeInput = Record.GetPhraseInput<UParseIntInput>(TEXT("NODE_INDEX"));
	UParseIntInput* PinInput = Record.GetPhraseInput<UParseIntInput>(TEXT("PIN_INDEX"));

	UParseStringInput* DefaultValueInput = Record.GetPhraseInput<UParseStringInput>(TEXT("DEFAULT_VALUE"));

	if (NodeInput == nullptr || PinInput == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("PinSetDefault: Invalid Node / Pin Inputs"));
		return;
	}

	if (DefaultValueInput == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("PinSetDefault: Invalid Value Input"));
		return;
	}

	UEdGraph* Graph = ActiveGraphEditor->GetCurrentGraph();

	TSharedRef<FGraphIndexer> Indexer = GetAssetRegistry()->GetGraphIndexer(Graph);

	UEdGraphPin* FoundPin = Indexer->GetPin(NodeInput->GetValue(), PinInput->GetValue());
	if (FoundPin == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("PinSetDefault: Pin Not Found"));
		return;
	}

	if (FoundPin->bDefaultValueIsReadOnly || FoundPin->bDefaultValueIsIgnored)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("PinSetDefault: Pin Default Value Is InAccessible"));
		return;
	}

	const UEdGraphSchema* GraphSchema = FoundPin->GetSchema();

	FString NewDefaultString = DefaultValueInput->GetValue();
	FText NewDefaultText = FText::FromString(NewDefaultString);

	FString DefaultSetLog = GraphSchema->IsPinDefaultValid(
		FoundPin, 
		NewDefaultString,
		nullptr, 
		NewDefaultText
	);

	if (!DefaultSetLog.IsEmpty())
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("PinSetDefault: Provided Value Is Invalid - %s"), *DefaultSetLog);
		return;
	}

	GraphSchema->TrySetDefaultValue(*FoundPin, NewDefaultString);
	GraphSchema->TrySetDefaultText(*FoundPin, NewDefaultText);
}

void UNodeInteractionLibrary::PinResetDefault(FParseRecord& Record)
{
	GET_CAST_ACTIVE_TAB_CONTENT(ActiveGraphEditor, SGraphEditor)

	// Get Phrase Inputs
	UParseIntInput* NodeInput = Record.GetPhraseInput<UParseIntInput>(TEXT("NODE_INDEX"));
	UParseIntInput* PinInput = Record.GetPhraseInput<UParseIntInput>(TEXT("PIN_INDEX"));

	if (NodeInput == nullptr || PinInput == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("PinSetDefault: Invalid Node / Pin Inputs"));
		return;
	}

	UEdGraph* Graph = ActiveGraphEditor->GetCurrentGraph();

	TSharedRef<FGraphIndexer> Indexer = GetAssetRegistry()->GetGraphIndexer(Graph);

	UEdGraphPin* FoundPin = Indexer->GetPin(NodeInput->GetValue(), PinInput->GetValue());
	if (FoundPin == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("PinSetDefault: Pin Not Found"));
		return;
	}

	if (!FoundPin->HasAnyConnections())
	{
		Graph->GetSchema()->ResetPinToAutogeneratedDefaultValue(FoundPin);
	}
}

TSharedPtr<IMenu> UNodeInteractionLibrary::NodeAddMenu(FParseRecord& Record) 
{
	GET_CAST_ACTIVE_TAB_CONTENT_RETURN(ActiveGraphEditor, SGraphEditor, TSharedPtr<IMenu>())

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
			GetFreeGraphViewportSpace(ActiveGraphEditor.Get()),
			nullptr,
			nullptr, 
			TArray<UEdGraphPin *>()
		);

		if (!ContextWidgetToFocus.IsValid())
		{
			UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("NodeAddMenu: Context Keyboard Focus Widget Not Found"));            
			return TSharedPtr<IMenu>();
		}

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
	GET_CAST_ACTIVE_TAB_CONTENT_RETURN(ActiveGraphEditor, SGraphEditor, TSharedPtr<IMenu>())

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

		/*
		FVector2D AddLocation = GetFreeGraphViewportSpace(ActiveGraphEditor.Get(), GraphPanel);
		if (AddLocation == FVector2D::ZeroVector)
		{
			AddLocation = GraphPanel->GetPastePosition();
		}
		*/

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
	GET_TOP_CONTEXT(Record, ContextMenu, UAccessibilityGraphEditorContext)

	UParseIntInput* IndexInput = Record.GetPhraseInput<UParseIntInput>(TEXT("SELECTION_INDEX"));
	if (IndexInput == nullptr)
		return;

	ContextMenu->SelectAction(IndexInput->GetValue());
}

void UNodeInteractionLibrary::NodeAddSearchAdd(FParseRecord& Record)
{
	GET_TOP_CONTEXT(Record, ContextMenu, UAccessibilityGraphEditorContext)

	UParseStringInput *SearchInput = Record.GetPhraseInput<UParseStringInput>(TEXT("SEARCH_PHRASE"));
	if (SearchInput == nullptr)
		return;

	ContextMenu->AppendFilterText(SearchInput->GetValue());
}

void UNodeInteractionLibrary::NodeAddSearchRemove(FParseRecord& Record)
{
	GET_TOP_CONTEXT(Record, ContextMenu, UAccessibilityGraphEditorContext);

	UParseIntInput* RemoveAmountInput = Record.GetPhraseInput<UParseIntInput>(TEXT("AMOUNT"));
	if (RemoveAmountInput == nullptr)
		return;

	ContextMenu->SetFilterText(
		EventUtils::RemoveWordsFromEnd(ContextMenu->GetFilterText(), RemoveAmountInput->GetValue())
	);
}

void UNodeInteractionLibrary::NodeAddSearchReset(FParseRecord& Record)
{
	GET_TOP_CONTEXT(Record, ContextMenu, UAccessibilityGraphEditorContext)

	ContextMenu->SetFilterText(TEXT(""));
}

void UNodeInteractionLibrary::NodeAddScroll(FParseRecord& Record)
{
	GET_TOP_CONTEXT(Record, ContextMenu, UAccessibilityGraphEditorContext)

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

void UNodeInteractionLibrary::SelectionNodeToggle(FParseRecord& Record)
{
	GET_CAST_ACTIVE_TAB_CONTENT(ActiveGraphEditor, SGraphEditor);

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

void UNodeInteractionLibrary::SelectionNodeAll(FParseRecord& Record)
{
	GET_CAST_ACTIVE_TAB_CONTENT(ActiveGraphEditor, SGraphEditor)

	ActiveGraphEditor->SelectAllNodes();
}

void UNodeInteractionLibrary::SelectionReset(FParseRecord &Record) {
	GET_CAST_ACTIVE_TAB_CONTENT(ActiveGraphEditor, SGraphEditor)

	ActiveGraphEditor->ClearSelectionSet();
}

void UNodeInteractionLibrary::SelectionMove(FParseRecord& Record)
{
	GET_CAST_ACTIVE_TAB_CONTENT(ActiveGraphEditor, SGraphEditor)

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
	GET_CAST_ACTIVE_TAB_CONTENT(ActiveGraphEditor, SGraphEditor)

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
	GET_CAST_ACTIVE_TAB_CONTENT(ActiveGraphEditor, SGraphEditor)

	ActiveGraphEditor->OnStraightenConnections();
}

void UNodeInteractionLibrary::SelectionComment(FParseRecord& Record)
{
	GET_CAST_ACTIVE_TAB_CONTENT(ActiveGraphEditor, SGraphEditor)

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

void UNodeInteractionLibrary::LocomotionCancel(FParseRecord& Record)
{
	GET_TOP_CONTEXT(Record, LocomotionContext, UAccessibilityGraphLocomotionContext);

	LocomotionContext->CancelLocomotion();
}

void UNodeInteractionLibrary::BlueprintCompile(FParseRecord& Record)
{
	GET_CAST_ACTIVE_TAB_CONTENT(ActiveGraphEditor, SGraphEditor)

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

UNodeInteractionLibrary::GridAttributes UNodeInteractionLibrary::GetGridAttributes(const SGraphPanel* Panel)
{
	if (Panel == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("GetGridAttributes: Invalid Panel"));
		return GridAttributes();
	}

	const float GraphSmallestGridSize = 8.0f;
	const float ZoomFactor = Panel->GetZoomAmount();
	float NominalGridSize = Panel->GetSnapGridSize();

	float InflationFactor = 1.0f;
	while (ZoomFactor * InflationFactor * NominalGridSize <= GraphSmallestGridSize)
	{
		InflationFactor *= 2.0f;
	}

	const float GridCellSize = NominalGridSize * ZoomFactor * InflationFactor;
	const float VisualGridCellSize = NominalGridSize * InflationFactor;

	UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("GridAttributes: GridCellSize: %f, NominalGridSize: %f, InflationFactor: %f, ZoomFactor: %f"), GridCellSize, NominalGridSize, InflationFactor, ZoomFactor);

	return {
		GridCellSize,
		VisualGridCellSize,
		NominalGridSize,
		InflationFactor,
		ZoomFactor
	};
}

void UNodeInteractionLibrary::SnapToGrid(const SGraphPanel* Panel, UEdGraphNode* Node)
{
	if (Panel == nullptr || Node == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("SnapToGrid: Invalid Panel / Node"));
		return;
	}

	GridAttributes GridAttr = GetGridAttributes(Panel);

	FVector2D SnappedPosition = FVector2D(
		FMath::RoundToInt(Node->NodePosX / GridAttr.GridCellSize) * GridAttr.GridCellSize,
		FMath::RoundToInt(Node->NodePosY / GridAttr.GridCellSize) * GridAttr.GridCellSize
	);

	Node->Modify();
	Node->NodePosX = SnappedPosition.X;
	Node->NodePosY = SnappedPosition.Y;
}

void UNodeInteractionLibrary::MoveOnGrid(const SGraphPanel* Panel, UEdGraphNode* Node, const FVector2D& MovementDelta)
{
	if (Panel == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("MoveOnGrid: Invalid Panel"));
		return;
	}

	GridAttributes GridAttr = GetGridAttributes(Panel);

	FVector2D ScaledMovementDelta = FVector2D(
		FMath::RoundToInt(MovementDelta.X * GridAttr.VisualGridCellSize),
		FMath::RoundToInt(MovementDelta.Y * GridAttr.VisualGridCellSize)
	);

	// Ensure Node Is Snapped to Grid
	FVector2D SnappedPosition = FVector2D(
		FMath::RoundToInt(Node->NodePosX / GridAttr.VisualGridCellSize) * GridAttr.VisualGridCellSize,
		FMath::RoundToInt(Node->NodePosY / GridAttr.VisualGridCellSize) * GridAttr.VisualGridCellSize
	);

	TSharedPtr<SGraphNode> NodeWidget = Panel ? Panel->GetNodeWidgetFromGuid(Node->NodeGuid) : TSharedPtr<SGraphNode>();
	if (NodeWidget.IsValid())
	{
		SNodePanel::SNode::FNodeSet NodeFilter;
		NodeWidget->MoveTo(SnappedPosition + ScaledMovementDelta, NodeFilter);
	}
	else
	{
		Node->Modify();
		Node->NodePosX = SnappedPosition.X + ScaledMovementDelta.X;
		Node->NodePosY = SnappedPosition.Y + ScaledMovementDelta.Y;
	}
}

void UNodeInteractionLibrary::SnapToGridCentre(const SGraphPanel* Panel, UEdGraphNode* Node)
{
	if (Panel == nullptr || Node == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("CenterToGrid: Invalid Panel / Node"));
		return;
	}

	GridAttributes GridAttr = GetGridAttributes(Panel);

	FVector2D SnappedPosition = FVector2D(
		FMath::RoundToInt(Node->NodePosX / GridAttr.GridCellSize) * GridAttr.GridCellSize,
		FMath::RoundToInt(Node->NodePosY / GridAttr.GridCellSize) * GridAttr.GridCellSize
	);

	Node->Modify();
	Node->NodePosX = SnappedPosition.X + (GridAttr.GridCellSize / 2);
	Node->NodePosY = SnappedPosition.Y + (GridAttr.GridCellSize / 2);
}

FVector2D GraphCoordToPanelCoord(FVector2D PanelCoord, const SGraphPanel* GraphPanel)
{
	if (GraphPanel == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("PanelCoordToGraphCoord: Invalid Graph Panel"));
		return FVector2D::ZeroVector;
	}

	return (PanelCoord - GraphPanel->GetViewOffset()) * GraphPanel->GetZoomAmount();
}

FVector2D UNodeInteractionLibrary::GetFreeGraphViewportSpace(const SGraphEditor* GraphEditor)
{
	if (GraphEditor == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("GetFreeGraphViewportSpace: Invalid Graph Editor"));
		return FVector2D::ZeroVector;
	}

	SGraphPanel* GraphPanel = GraphEditor->GetGraphPanel();

	TArray<UEdGraphNode*> GraphNodes;
	GetNodesInViewport(GraphEditor, GraphPanel, GraphNodes);

	FVector2D GridResolution;
	FVector2D PanelSize = GraphPanel->GetCachedGeometry().GetLocalSize();
	{
		FVector2D AverageNodeSize(300, 250);

		FVector2D GraphViewSize = GraphPanel->PanelCoordToGraphCoord(PanelSize) - GraphPanel->PanelCoordToGraphCoord(FVector2D::ZeroVector);
		constexpr float BufferScalar = 1.3f;

		GridResolution = FVector2D(
			FMath::CeilToInt(GraphViewSize.X / (AverageNodeSize.X * BufferScalar)),
			FMath::CeilToInt(GraphViewSize.Y / (AverageNodeSize.Y * BufferScalar))
		);

		UE_LOG(LogOpenAccessibility, Log, TEXT("Calculated Node-Based Density Grid Resolution: %s"), *GridResolution.ToString());

		FVector2D DefaultGridResolution(6, 4);
		if (GridResolution.ComponentwiseAllLessThan(DefaultGridResolution))
			GridResolution = DefaultGridResolution;
	}

	FVector2D CellSize = PanelSize / GridResolution;

	TArray<int32> DensityGrid;
	DensityGrid.Init(0, GridResolution.X * GridResolution.Y);

	// Build Density Per Cell
	for (auto& GraphNode : GraphNodes)
	{
		for (int Y = 0; Y < 2; ++Y)
		{
			for (int X = 0; X < 2; ++X)
			{
				FVector2D NodeCorner = GraphCoordToPanelCoord(FVector2D(GraphNode->NodePosX + (X * GraphNode->NodeWidth), GraphNode->NodePosY + (Y * GraphNode->NodeHeight)), GraphPanel);

				int32 xIndex = FMath::Clamp(FMath::RoundToInt(NodeCorner.X / CellSize.X), 0, GridResolution.X - 1);
				int32 yIndex = FMath::Clamp(FMath::RoundToInt(NodeCorner.Y / CellSize.Y), 0, GridResolution.Y - 1);

				DensityGrid[xIndex + yIndex * GridResolution.X]++;
			}
		}
	}

	int32 minCount = INT32_MAX;
	FVector2D OptimalPosition = FVector2D::ZeroVector;

	for (int Y = 0; Y < GridResolution.Y; ++Y)
	{
		for (int X = 0; X < GridResolution.X; ++X)
		{
			// Include Direct Neighbour Into Density
			int32 CellCount = DensityGrid[X + Y * GridResolution.X];

			// Include Direct Neighbour Into Density
			// Includes: Up, Down, Left and Right.
			CellCount += X > 0 ? DensityGrid[(X - 1) + Y * GridResolution.X] : 0;
			CellCount += X < GridResolution.X - 1 ? DensityGrid[(X + 1) + Y * GridResolution.X] : 0;
			CellCount += Y > 0 ? DensityGrid[X + (Y - 1) * GridResolution.X] : 0;
			CellCount += Y < GridResolution.Y - 1 ? DensityGrid[X + (Y + 1) * GridResolution.X] : 0;

			if (CellCount > minCount)
				continue;

			FVector2D PossibleOptimalPosition = FVector2D(X + 0.25f, Y + 0.25f) * CellSize;

			if (FVector2D::Distance(PossibleOptimalPosition, PanelSize / 2) < FVector2D::Distance(OptimalPosition, PanelSize / 2))
			{
				minCount = CellCount;
				OptimalPosition = PossibleOptimalPosition;
			}
		}
	}

	return OptimalPosition != FVector2D::ZeroVector ? GraphPanel->PanelCoordToGraphCoord(OptimalPosition) : GraphPanel->GetPastePosition();
}

int32 UNodeInteractionLibrary::GetNodesInViewport(const SGraphEditor* GraphEditor, const SGraphPanel* GraphPanel, TArray<UEdGraphNode*>& GraphNodes)
{
	if (GraphEditor == nullptr || GraphPanel == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("GetNodesInViewport: Invalid Graph Widget is nullptr"));
		return INDEX_NONE;
	}

	UEdGraph* Graph = GraphEditor->GetCurrentGraph();

	FVector2D NodeTopLeft = FVector2D::ZeroVector;
	FVector2D NodeBotRight = FVector2D::ZeroVector;
	for (UEdGraphNode* Node : Graph->Nodes)
	{
		NodeTopLeft = FVector2D(Node->NodePosX, Node->NodePosY);
		NodeBotRight = FVector2D(Node->NodePosX + Node->NodeWidth, Node->NodePosY + Node->NodeHeight);

		if (const_cast<SGraphPanel*>(GraphPanel)->IsRectVisible(NodeTopLeft, NodeBotRight))
		{
			GraphNodes.Add(Node);
		}
	}

	return GraphNodes.Num();
}
