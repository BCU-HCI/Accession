#include "PhraseEvents/NodeInteractionLibrary.h"

#include "SGraphPanel.h"

#include "PhraseTree/Containers/Input/InputContainers.h"
#include "AccessibilityWrappers/AccessibilityAddNodeContextMenu.h"

void UNodeInteractionLibrary::MoveNode(FParseRecord &Record)
{
	GET_ACTIVE_TAB(ActiveGraphEditor, SGraphEditor)

	UParseIntInput* IndexInput = Record.GetPhraseInput<UParseIntInput>(TEXT("INDEX"));
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

	FVector2D NodePositon = FVector2D(Node->NodePosX, Node->NodePosY);
	switch (EPhrase2DDirectionalInput(DirectionInput->GetValue()))
	{
		case EPhrase2DDirectionalInput::UP:
            NodePositon.Y -= AmountInput->GetValue();
			break;

		case EPhrase2DDirectionalInput::DOWN:
			NodePositon.Y += AmountInput->GetValue();
			break;

		case EPhrase2DDirectionalInput::LEFT:
			NodePositon.X -= AmountInput->GetValue();
			break;

		case EPhrase2DDirectionalInput::RIGHT:
			NodePositon.X += AmountInput->GetValue();
			break;

		default:
			UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("MoveNode: Invalid Direction"));
			return;
	}

	Node->Modify();
    Node->NodePosX = NodePositon.X;
	Node->NodePosY = NodePositon.Y;
}

void UNodeInteractionLibrary::DeleteNode(FParseRecord& Record)
{
	GET_ACTIVE_TAB(ActiveGraphEditor, SGraphEditor)

	UParseIntInput* IndexInput = Record.GetPhraseInput<UParseIntInput>(TEXT("INDEX"));
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

TSharedPtr<IMenu> UNodeInteractionLibrary::NodeAddMenu(FParseRecord& Record) {
	GET_ACTIVE_TAB(ActiveGraphEditor, SGraphEditor)

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
		else UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("NodeAddMenu: IMenu Could Not Be Found In Widget Path"))
	}
}

TSharedPtr<IMenu>
UNodeInteractionLibrary::NodeAddPinMenu(FParseRecord &Record) {
GET_ACTIVE_TAB(ActiveGraphEditor, SGraphEditor)

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
		else UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("NodeAddMenu: IMenu Could Not Be Found In Widget Path"))
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

void UNodeInteractionLibrary::NodeAddSearchNew(FParseRecord& Record)
{
	GET_TOP_CONTEXT(Record, ContextMenu, UAccessibilityAddNodeContextMenu)

	UParseStringInput* SearchInput = Record.GetPhraseInput<UParseStringInput>(TEXT("SEARCH_PHRASE"));
	if (SearchInput == nullptr)
		return;

	ContextMenu->SetFilterText(SearchInput->GetValue());
}

void UNodeInteractionLibrary::NodeAddSearchAdd(FParseRecord& Record)
{
	GET_TOP_CONTEXT(Record, ContextMenu, UAccessibilityAddNodeContextMenu)

	UParseStringInput *SearchInput = Record.GetPhraseInput<UParseStringInput>(TEXT("SEARCH_PHRASE"));
	if (SearchInput == nullptr)
		return;

	ContextMenu->AppendFilterText(SearchInput->GetValue());
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

void UNodeInteractionLibrary::SelectionAdd(FParseRecord& Record)
{
	GET_ACTIVE_TAB(ActiveGraphEditor, SGraphEditor)

	UParseIntInput* IndexInput = Record.GetPhraseInput<UParseIntInput>(TEXT("NODE_INDEX"));
	if (IndexInput == nullptr)
		return;

	TSharedRef<FGraphIndexer> Indexer = GetAssetRegistry()->GetGraphIndexer(ActiveGraphEditor->GetCurrentGraph());
	
	UEdGraphNode *Node = Indexer->GetNode(IndexInput->GetValue());
	if (Node == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("SelectionAdd: Node Not Found"));
		return;
	}

	ActiveGraphEditor->SetNodeSelection(Node, true);
}

void UNodeInteractionLibrary::SelectionRemove(FParseRecord& Record)
{
	GET_ACTIVE_TAB(ActiveGraphEditor, SGraphEditor)

	UParseIntInput* IndexInput = Record.GetPhraseInput<UParseIntInput>(TEXT("NODE_INDEX"));
	if (IndexInput == nullptr)
		return;

	TSharedRef<FGraphIndexer> Indexer = GetAssetRegistry()->GetGraphIndexer(ActiveGraphEditor->GetCurrentGraph());

	UEdGraphNode *Node = Indexer->GetNode(IndexInput->GetValue());
	if (Node == nullptr)
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("SelectionRemove: Node Not Found"));
		return;
	}

	ActiveGraphEditor->SetNodeSelection(Node, false);
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
