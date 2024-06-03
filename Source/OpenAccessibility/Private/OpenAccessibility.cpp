// Copyright Epic Games, Inc. All Rights Reserved.

#include "OpenAccessibility.h"
#include "OpenAccessibilityCommunication.h"
#include "OpenAccessibilityLogging.h"

#include "PhraseTree/PhraseNode.h"
#include "PhraseTree/PhraseInputNode.h"
#include "PhraseTree/PhraseStringInputNode.h"
#include "PhraseTree/PhraseDirectionalInputNode.h"
#include "PhraseTree/PhraseContextNode.h"
#include "PhraseTree/PhraseContextMenuNode.h"
#include "PhraseTree/PhraseEventNode.h"

#include "PhraseTree/Containers/Input/UParseInput.h"
#include "PhraseTree/Containers/Input/UParseIntInput.h"
#include "PhraseTree/Containers/Input/UParseStringInput.h"
#include "PhraseTree/Containers/Input/UParseEnumInput.h"
#include "PhraseTree/Containers/Input/InputContainers.h"

#include "TranscriptionVisualizer.h"
#include "AccessibilityWrappers/AccessibilityWindowToolbar.h"

#include "GraphActionNode.h"
#include "SGraphPanel.h"
#include "Widgets/Text/SMultiLineEditableText.h"
#include "Widgets/Input/SSearchBox.h"

#include "Framework/Docking/TabManager.h"
#include "Logging/StructuredLog.h"

/// <summary>
/// Obtains the Active Unreal Tab, if available, and Casts It To the Provided Type.
/// </summary>
/// <param name="ActiveContainerName">- The Name of the SharedPtr To Store The Found Tab In.</param>
/// <param name="InActiveTabType">- The Type of the Tab To Cast To.</param>
#define GET_ACTIVE_TAB( ActiveContainerName, InActiveTabType, ...) TSharedPtr< InActiveTabType > ActiveContainerName; {\
		TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab(); \
		if (!ActiveTab.IsValid()) \
		{ \
			UE_LOG(LogOpenAccessibility, Display, TEXT("GET_ACTIVE_TAB: NO ACTIVE TAB FOUND.")); \
			return; \
		} \
		ActiveContainerName = StaticCastSharedPtr< InActiveTabType >(ActiveTab->GetContent().ToSharedPtr()); \
		if (!ActiveContainerName.IsValid()) \
		{ \
			UE_LOG(LogOpenAccessibility, Display, TEXT("GET_ACTIVE_TAB: CURRENT ACTIVE TAB IS NOT OF TYPE - %s"), #InActiveTabType); \
			return; \
		} \
	}

#define LOCTEXT_NAMESPACE "FOpenAccessibilityModule"

void FOpenAccessibilityModule::StartupModule()
{
	UE_LOG(LogOpenAccessibility, Display, TEXT("OpenAccessibilityModule::StartupModule()"));

	// Create the Asset Registry
	AssetAccessibilityRegistry = MakeShared<FAssetAccessibilityRegistry, ESPMode::ThreadSafe>();

	// Register the Accessibility Node Factory
	AccessibilityNodeFactory = MakeShared<FAccessibilityNodeFactory, ESPMode::ThreadSafe>();
	FEdGraphUtilities::RegisterVisualNodeFactory(AccessibilityNodeFactory);

	// Construct the Manager for Toolbar Accessibility
	// ToolbarAccessibility = NewObject<UAccessibilityWindowToolbar>();
	// ToolbarAccessibility->AddToRoot();

	// Register Console Commands
	RegisterConsoleCommands();

	BindGraphInteractionBranch();
	BindLocalizedInteractionBranch();

	CreateTranscriptionVisualization();
}

void FOpenAccessibilityModule::ShutdownModule()
{
	UE_LOG(LogOpenAccessibility, Display, TEXT("OpenAccessibilityModule::ShutdownModule()"));

	UnregisterConsoleCommands();
}

void FOpenAccessibilityModule::BindLocalizedInteractionBranch()
{
	// Localized Object Interaction

	TSharedPtr<FPhraseEventNode> MoveViewportEventNode = MakeShared<FPhraseEventNode>();
	MoveViewportEventNode->OnPhraseParsed.BindLambda([this](FParseRecord& Record) {
		TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
		if (!ActiveTab.IsValid())
		{
			return;
		}

		SWidget* ActiveEditorViewport = ActiveTab->GetContent().ToSharedPtr().Get();
		if (ActiveEditorViewport == nullptr)
		{
			return;
		}

		// Get Inputs
		UParseEnumInput* MoveDirection = Record.GetPhraseInput<UParseEnumInput>(TEXT("DIRECTION"));
		UParseIntInput* MoveAmount = Record.GetPhraseInput<UParseIntInput>(TEXT("AMOUNT"));

		const FString ActiveEditorViewportType = ActiveEditorViewport->GetTypeAsString();
		if (ActiveEditorViewportType == "SGraphEditor")
		{
			SGraphEditor* GraphViewport = (SGraphEditor*) ActiveEditorViewport;

			FVector2D CurrViewLocation;
			float CurrZoomAmount;
			GraphViewport->GetViewLocation(CurrViewLocation, CurrZoomAmount);

			UE_LOG(LogOpenAccessibility, Display, TEXT("MoveViewportEventNode: Curr Zoom Amount: %d"), CurrZoomAmount);
		
			switch (EPhraseDirectionalInput(MoveDirection->GetValue()))
			{
				case EPhraseDirectionalInput::UP:
					CurrViewLocation.Y -= MoveAmount->GetValue();
					break;

				case EPhraseDirectionalInput::DOWN:
					CurrViewLocation.Y += MoveAmount->GetValue();
					break;

				case EPhraseDirectionalInput::LEFT:
					CurrViewLocation.X -= MoveAmount->GetValue();
					break;

				case EPhraseDirectionalInput::RIGHT:
					CurrViewLocation.X += MoveAmount->GetValue();
					break;

				default:
					UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- Invalid Direction --"));
					return;
			}

			GraphViewport->SetViewLocation(CurrViewLocation, CurrZoomAmount);
		}

		// Add Other Viewport Impl Here...
	});

	TSharedPtr<FPhraseEventNode> ZoomViewportEventNode = MakeShared<FPhraseEventNode>();
	ZoomViewportEventNode->OnPhraseParsed.BindLambda([this](FParseRecord& Record) {
		TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
		if (!ActiveTab.IsValid())
		{
			return;
		}

		SWidget* ActiveEditorViewport = ActiveTab->GetContent().ToSharedPtr().Get();
		if (ActiveEditorViewport == nullptr)
		{
			return;
		}

		// Get Inputs
		UParseEnumInput* ZoomDirection = Record.GetPhraseInput<UParseEnumInput>(TEXT("DIRECTION"));
		UParseIntInput* ZoomAmount = Record.GetPhraseInput<UParseIntInput>(TEXT("AMOUNT"));

		const FString ActiveEditorViewportType = ActiveEditorViewport->GetTypeAsString();
		if (ActiveEditorViewportType == "SGraphEditor")
		{
			SGraphEditor* GraphViewport = (SGraphEditor*)ActiveEditorViewport;

			FVector2D CurrViewLocation;
			float CurrZoomAmount;
			GraphViewport->GetViewLocation(CurrViewLocation, CurrZoomAmount);

			UE_LOG(LogOpenAccessibility, Display, TEXT("ZoomViewportEventNode: Curr Zoom Amount: %d"), CurrZoomAmount);

			switch (EPhraseDirectionalInput(ZoomDirection->GetValue()))
			{
				case EPhraseDirectionalInput::UP:
					CurrZoomAmount += ZoomAmount->GetValue();
					break;

				case EPhraseDirectionalInput::DOWN:
					CurrZoomAmount -= ZoomAmount->GetValue();
					break;

				default:
					UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- Invalid Direction --"));
					return;
			}

			GraphViewport->SetViewLocation(CurrViewLocation, CurrZoomAmount);
		}

		// Add Other Viewport Impl Here...
	});

	TSharedPtr<FPhraseEventNode> IndexFocusEventNode = MakeShared<FPhraseEventNode>();
	IndexFocusEventNode->OnPhraseParsed.BindLambda([this](FParseRecord& Record) {
		TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
		if (!ActiveTab.IsValid())
		{
			return;
		}

		SWidget* ActiveEditorViewport = ActiveTab->GetContent().ToSharedPtr().Get();
		if (ActiveEditorViewport == nullptr)
		{
			return;
		}

		// Get Inputs
		UParseIntInput* IndexToFocus = Record.GetPhraseInput<UParseIntInput>("INDEX");

		const FString ActiveEditorViewportType = ActiveEditorViewport->GetTypeAsString();
		if (ActiveEditorViewportType == "SGraphEditor")
		{
			SGraphEditor* GraphViewport = (SGraphEditor*)ActiveEditorViewport;

			TSharedRef<FGraphIndexer> IndexerForGraph = AssetAccessibilityRegistry->GetGraphIndexer(GraphViewport->GetCurrentGraph());
			UEdGraphNode* GraphNode = IndexerForGraph->GetNode(IndexToFocus->GetValue());
			
			GraphViewport->JumpToNode(GraphNode, false, true);
		}
	});

	// -----

	// Localized Input Interaction (TextBoxes, etc.)

	TSharedPtr<FPhraseEventNode> TextInputAppendEventNode = MakeShared<FPhraseEventNode>();
	TextInputAppendEventNode->OnPhraseParsed.BindLambda([this](FParseRecord& Record) {
		FSlateApplication& SlateApp = FSlateApplication::Get();
		if (!SlateApp.IsInitialized())
			return;

		UParseStringInput* PhraseInput = Record.GetPhraseInput<UParseStringInput>(TEXT("PHRASE_TO_ADD"));
		if (PhraseInput == nullptr)
			return;

		TSharedPtr<SWidget> KeyboardFocusedWidget = SlateApp.GetKeyboardFocusedWidget();
		if (!KeyboardFocusedWidget.IsValid())
			return;

		FString KeyboardFocusedWidgetType = KeyboardFocusedWidget->GetTypeAsString();

		if (KeyboardFocusedWidgetType == "SEditableText")
		{
			TSharedPtr<SEditableText> EditableText = StaticCastSharedPtr<SEditableText>(KeyboardFocusedWidget);

			FString CurrText = EditableText->GetText().ToString();
			EditableText->SetText(
				FText::FromString(CurrText.TrimStartAndEnd() + TEXT(" ") + PhraseInput->GetValue())
			);

			return;
		}
		else if (KeyboardFocusedWidgetType == "SMultiLineEditableText")
		{
			TSharedPtr<SMultiLineEditableText> EditableMultiLineText = StaticCastSharedPtr<SMultiLineEditableText>(KeyboardFocusedWidget);

			FString CurrText = EditableMultiLineText->GetText().ToString();

			EditableMultiLineText->SetText(
				FText::FromString(CurrText.TrimStartAndEnd() + TEXT(" ") + PhraseInput->GetValue())
			);

			return;
		}
		else UE_LOG(LogOpenAccessibility, Display, TEXT("Found TextBox Not Of Editable Type"));
	});

	TSharedPtr<FPhraseEventNode> TextInputRemoveEventNode = MakeShared<FPhraseEventNode>();
	TextInputRemoveEventNode->OnPhraseParsed.BindLambda([this](FParseRecord& Record) {
		FSlateApplication& SlateApp = FSlateApplication::Get();
		if (!SlateApp.IsInitialized())
			return;

		TSharedPtr<SEditableText> ActiveTextBox = StaticCastSharedPtr<SEditableText>(SlateApp.GetKeyboardFocusedWidget());
		if (!ActiveTextBox.IsValid())
			return;

		UParseIntInput* AmountToRemove = Record.GetPhraseInput<UParseIntInput>(TEXT("AMOUNT"));
		if (AmountToRemove == nullptr)
			return;

		FString TextBoxString = ActiveTextBox->GetText().ToString();

		{
			TArray<FString> SplitTextBoxString;
			TextBoxString.ParseIntoArrayWS(SplitTextBoxString);

			int RemovedAmount = 0;
			int CurrentIndex = SplitTextBoxString.Num() - 1;
			while (RemovedAmount < AmountToRemove->GetValue())
			{
				if (SplitTextBoxString.IsEmpty())
					break;

				SplitTextBoxString.RemoveAt(CurrentIndex--);
				RemovedAmount++;
			}

			if (SplitTextBoxString.Num() > 0)
				TextBoxString = FString::Join(SplitTextBoxString, TEXT(" "));
			else TextBoxString = TEXT("");
		}

		ActiveTextBox->SetText(
			FText::FromString(TextBoxString)
		);
	});

	TSharedPtr<FPhraseEventNode> TextInputResetEventNode = MakeShared<FPhraseEventNode>();
	TextInputResetEventNode->OnPhraseParsed.BindLambda([this](FParseRecord& Record) {
		FSlateApplication& SlateApp = FSlateApplication::Get();
		if (!SlateApp.IsInitialized())
			return;

		TSharedPtr<SEditableText> ActiveTextBox = StaticCastSharedPtr<SEditableText>(SlateApp.GetKeyboardFocusedWidget());
		if (!ActiveTextBox.IsValid())
			return;

		ActiveTextBox->SetText(FText::FromString(TEXT("")));
	});

	TSharedPtr<FPhraseEventNode> TextInputExitEventNode = MakeShared<FPhraseEventNode>();
	TextInputExitEventNode->OnPhraseParsed.BindLambda([this](FParseRecord& Record) {
		FSlateApplication& SlateApp = FSlateApplication::Get();
		if (!SlateApp.IsInitialized())
			return;

		TSharedPtr<SEditableTextBox> ActiveTextBox = StaticCastSharedPtr<SEditableTextBox>(SlateApp.GetKeyboardFocusedWidget());
		if (!ActiveTextBox.IsValid())
			return;

		SlateApp.ClearKeyboardFocus(EFocusCause::Cleared);
	});

	// -----

	FOpenAccessibilityCommunicationModule::Get().PhraseTree->BindBranches(
		TPhraseNodeArray {
			MakeShared<FPhraseNode>(TEXT("VIEW"),
			TPhraseNodeArray{

				MakeShared<FPhraseNode>(TEXT("MOVE"),
				TPhraseNodeArray {

					MakeShared<FPhrase2DDirectionalInputNode>(TEXT("DIRECTION"),
					TPhraseNodeArray {

						MakeShared<FPhraseInputNode<int32>>(TEXT("AMOUNT"),
						TPhraseNodeArray {
							MoveViewportEventNode
						})

					})

				}),

				MakeShared<FPhraseNode>(TEXT("ZOOM"),
				TPhraseNodeArray {

					MakeShared<FPhrase2DDirectionalInputNode>(TEXT("DIRECTION"),
					TPhraseNodeArray {

						MakeShared<FPhraseInputNode<int32>>(TEXT("AMOUNT"),
						TPhraseNodeArray {
							ZoomViewportEventNode
						})
					})
				}),

				MakeShared<FPhraseNode>(TEXT("FOCUS"),
				TPhraseNodeArray {

					MakeShared<FPhraseInputNode<int32>>(TEXT("INDEX"),
					TPhraseNodeArray {
						IndexFocusEventNode
					})
				})
			}),

			MakeShared<FPhraseNode>(TEXT("INPUT"),
			TPhraseNodeArray {
				
				MakeShared<FPhraseNode>(TEXT("ADD"),
				TPhraseNodeArray {

					MakeShared<FPhraseStringInputNode>(TEXT("PHRASE_TO_ADD"),
					TPhraseNodeArray {
						TextInputAppendEventNode
					})

				}),

				MakeShared<FPhraseNode>(TEXT("REMOVE"),
				TPhraseNodeArray {

					MakeShared<FPhraseInputNode<int32>>(TEXT("AMOUNT"),
					TPhraseNodeArray {
						TextInputRemoveEventNode
					})

				}),

				MakeShared<FPhraseNode>(TEXT("RESET"),
				TPhraseNodeArray {
					TextInputResetEventNode
				}),

				MakeShared<FPhraseNode>(TEXT("EXIT"),
				TPhraseNodeArray {
					TextInputExitEventNode
				})

			})
		}
	);
}

void FOpenAccessibilityModule::BindGraphInteractionBranch()
{
	// Node Events
	TSharedPtr<FPhraseEventNode> MoveEventNode = MakeShared<FPhraseEventNode>();
	MoveEventNode->OnPhraseParsed.BindLambda([this](FParseRecord& Record) {
		TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
		if (!ActiveTab.IsValid())
		{
			return;
		}

		SGraphEditor* ActiveGraphEditor = (SGraphEditor*)ActiveTab->GetContent().ToSharedPtr().Get();

		if (ActiveGraphEditor != nullptr)
		{
			// Get Inputs
			UParseIntInput* NodeIndex = Record.GetPhraseInput<UParseIntInput>(TEXT("NODE_INDEX"));
			UParseEnumInput* MoveDirection = Record.GetPhraseInput<UParseEnumInput>(TEXT("DIRECTION"));
			UParseIntInput* MoveAmount = Record.GetPhraseInput<UParseIntInput>(TEXT("AMOUNT"));

			UEdGraphNode* GraphNode = AssetAccessibilityRegistry->GraphAssetIndex[ActiveGraphEditor->GetCurrentGraph()->GraphGuid]->GetNode(
				NodeIndex->GetValue()
			);

			if (GraphNode == nullptr)
			{
				UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- Invalid Node Index --"));
				return;
			}

			switch (EPhrase2DDirectionalInput(MoveDirection->GetValue()))
			{
			case EPhrase2DDirectionalInput::UP:
				GraphNode->NodePosY -= MoveAmount->GetValue();
				break;

			case EPhrase2DDirectionalInput::DOWN:
				GraphNode->NodePosY += MoveAmount->GetValue();
				break;

			case EPhrase2DDirectionalInput::LEFT:
				GraphNode->NodePosX -= MoveAmount->GetValue();
				break;

			case EPhrase2DDirectionalInput::RIGHT:
				GraphNode->NodePosX += MoveAmount->GetValue();
				break;

			default:
				UE_LOG(LogOpenAccessibility, Display, TEXT("Provided Direction Is In-Valid"));
				return;
			}
		}
		else
		{
			UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- Active Tab Not SGraphEditor --"));
			return;
		}
		});

	TSharedPtr<FPhraseEventNode> DeleteEventNode = MakeShared<FPhraseEventNode>();
	DeleteEventNode->OnPhraseParsed.BindLambda([this](FParseRecord& Record) {
		GET_ACTIVE_TAB(ActiveGraphEditor, SGraphEditor);

		// Get Node Index
		UParseIntInput* NodeIndex = Record.GetPhraseInput<UParseIntInput>(TEXT("NODE_INDEX"));
		if (NodeIndex == nullptr)
			return;

		UEdGraph* ActiveGraph = ActiveGraphEditor->GetCurrentGraph();
		if (ActiveGraph == nullptr)
			return;	

		// Get Graph Indexer
		TSharedRef<FGraphIndexer> IndexerForGraph = AssetAccessibilityRegistry->GetGraphIndexer(ActiveGraph);

		UEdGraphNode* GraphNode = IndexerForGraph->GetNode(NodeIndex->GetValue());
		if (GraphNode == nullptr)
			return;
		else GraphNode->DestroyNode();
	});

	// -----

	// Pin Events
	TSharedPtr<FPhraseEventNode> PinConnectEventNode = MakeShared<FPhraseEventNode>();
	PinConnectEventNode->OnPhraseParsed.BindLambda([this](FParseRecord& Record) {

		TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
		if (!ActiveTab.IsValid())
		{
			UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- No Active Tab --"));
			return;
		}

		SGraphEditor* ActiveGraphEditor = (SGraphEditor*)ActiveTab->GetContent().ToSharedPtr().Get();
		if (ActiveGraphEditor == nullptr)
		{
			UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- Active Tab Not SGraphEditor --"));
			return;
		}

		UEdGraph* CurrentGraph = ActiveGraphEditor->GetCurrentGraph();
		if (CurrentGraph == nullptr)
		{
			UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- No Current Graph --"));
			return;
		}

		TSharedPtr<FGraphIndexer> IndexerForGraph = AssetAccessibilityRegistry->GetGraphIndexer(ActiveGraphEditor->GetCurrentGraph());

		// Get Inputs
		TArray<UParseInput*> NodeInputs = Record.GetPhraseInputs(TEXT("NODE_INDEX"));

		TArray<UParseInput*> PinInputs = Record.GetPhraseInputs(TEXT("PIN_INDEX"));
		if (NodeInputs.Num() < 2 || PinInputs.Num() < 2)
		{
			UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- Invalid Inputs Length --"));
			return;
		}

		UEdGraphPin* SourcePin = IndexerForGraph->GetPin(
			Cast<UParseIntInput>(NodeInputs[0])->GetValue(),
			Cast<UParseIntInput>(PinInputs[0])->GetValue()
		);

		UEdGraphPin* TargetPin = IndexerForGraph->GetPin(
			Cast<UParseIntInput>(NodeInputs[1])->GetValue(),
			Cast<UParseIntInput>(PinInputs[1])->GetValue()
		);

		if (SourcePin == nullptr || TargetPin == nullptr)
		{
			UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- Invalid Pins --"));
			return;
		}


		if (CurrentGraph->GetSchema()->TryCreateConnection(SourcePin, TargetPin))
		{
			UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Success -- Pins Connected --"))
		}
		});

	TSharedPtr<FPhraseEventNode> PinDisconnectEventNode = MakeShared<FPhraseEventNode>();
	PinDisconnectEventNode->OnPhraseParsed.BindLambda([this](FParseRecord& Record) {

		TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
		if (!ActiveTab.IsValid())
		{
			UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- No Active Tab --"));
			return;
		}

		SGraphEditor* ActiveGraphEditor = (SGraphEditor*)ActiveTab->GetContent().ToSharedPtr().Get();
		if (ActiveGraphEditor == nullptr)
		{
			UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- Active Tab Not SGraphEditor --"));
			return;
		}
		UEdGraph* CurrentGraph = ActiveGraphEditor->GetCurrentGraph();
		if (CurrentGraph == nullptr)
		{
			UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- No Current Graph --"));
			return;
		}

		TSharedPtr<FGraphIndexer> IndexerForGraph = AssetAccessibilityRegistry->GetGraphIndexer(ActiveGraphEditor->GetCurrentGraph());

		// Get Inputs
		TArray<UParseInput*> NodeInputs = Record.GetPhraseInputs(TEXT("NODE_INDEX"));

		TArray<UParseInput*> PinInputs = Record.GetPhraseInputs(TEXT("PIN_INDEX"));


		if (NodeInputs.Num() < 2 || PinInputs.Num() < 2)
		{
			UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- Invalid Inputs Length --"));
			return;
		}

		UEdGraphPin* SourcePin = IndexerForGraph->GetPin(
			Cast<UParseIntInput>(NodeInputs[0])->GetValue(),
			Cast<UParseIntInput>(PinInputs[0])->GetValue()
		);

		UEdGraphPin* TargetPin = IndexerForGraph->GetPin(
			Cast<UParseIntInput>(NodeInputs[1])->GetValue(),
			Cast<UParseIntInput>(PinInputs[1])->GetValue()
		);

		if (SourcePin == nullptr || TargetPin == nullptr)
		{
			UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- Invalid Pins --"));
			return;
		}

		CurrentGraph->GetSchema()->BreakSinglePinLink(SourcePin, TargetPin);
		});

	TDelegate<void(int32 IndexInput)> NodeIndexFocusEvent;
	NodeIndexFocusEvent.BindLambda([this](int32 NodeIndex) {
		TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
		if (!ActiveTab.IsValid())
		{
			return;
		}

		SGraphEditor* ActiveGraphEditor = (SGraphEditor*)ActiveTab->GetContent().ToSharedPtr().Get();
		if (ActiveGraphEditor == nullptr)
		{
			UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- Active Tab Not SGraphEditor --"));
			return;
		}

		TSharedRef<FGraphIndexer> GraphIndexer = AssetAccessibilityRegistry->GetGraphIndexer(ActiveGraphEditor->GetCurrentGraph());

		UEdGraphNode* Node = GraphIndexer->GetNode(NodeIndex);
		if (Node == nullptr)
		{
			UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- Invalid Node Index --"));
			return;
		}

		ActiveGraphEditor->ClearSelectionSet();
		ActiveGraphEditor->SetNodeSelection(Node, true);
		});
	// ------

	// Node Selection Events

	TSharedPtr<FPhraseEventNode> NodeSelectionAddNode = MakeShared<FPhraseEventNode>();
	NodeSelectionAddNode->OnPhraseParsed.BindLambda([this](FParseRecord& Record) {
		GET_ACTIVE_TAB( ActiveGraphEditor, SGraphEditor );

		UParseIntInput* NodeIndexInput = Record.GetPhraseInput<UParseIntInput>(TEXT("NODE_INDEX"));
		if (NodeIndexInput == nullptr)
			return;

		TSharedRef<FGraphIndexer> IndexerForGraph = AssetAccessibilityRegistry->GetGraphIndexer(ActiveGraphEditor->GetCurrentGraph());

		UEdGraphNode* GraphNode = IndexerForGraph->GetNode(NodeIndexInput->GetValue());
		if (GraphNode == nullptr)
			return;

		ActiveGraphEditor->SetNodeSelection(GraphNode, true);
	});

	TSharedPtr<FPhraseEventNode> NodeSelectionRemoveNode = MakeShared<FPhraseEventNode>();
	NodeSelectionRemoveNode->OnPhraseParsed.BindLambda([this](FParseRecord& Record) {
		GET_ACTIVE_TAB( ActiveGraphEditor, SGraphEditor );

		UParseIntInput* NodeIndexInput = Record.GetPhraseInput<UParseIntInput>(TEXT("NODE_INDEX"));
		if (NodeIndexInput == nullptr)
			return;

		TSharedRef<FGraphIndexer> IndexerForGraph = AssetAccessibilityRegistry->GetGraphIndexer(ActiveGraphEditor->GetCurrentGraph());

		UEdGraphNode* GraphNode = IndexerForGraph->GetNode(NodeIndexInput->GetValue());
		if (GraphNode == nullptr)
			return;

		ActiveGraphEditor->SetNodeSelection(GraphNode, false);
	});

	TSharedPtr<FPhraseEventNode> NodeSelectionReset = MakeShared<FPhraseEventNode>();
	NodeSelectionReset->OnPhraseParsed.BindLambda([this](FParseRecord& Record) {
		GET_ACTIVE_TAB(ActiveGraphEditor, SGraphEditor);

		ActiveGraphEditor->ClearSelectionSet();
	});

	TSharedPtr<FPhraseEventNode> NodeSelectionMove = MakeShared<FPhraseEventNode>();
	NodeSelectionMove->OnPhraseParsed.BindLambda([this](FParseRecord& Record) {
		GET_ACTIVE_TAB(ActiveGraphEditor, SGraphEditor);

		UParseEnumInput* Direction = Record.GetPhraseInput<UParseEnumInput>(TEXT("DIRECTION"));
		if (Direction == nullptr)
			return;

		UParseIntInput* Amount = Record.GetPhraseInput<UParseIntInput>(TEXT("AMOUNT"));
		if (Amount == nullptr)
			return;

		for (UObject* Node : ActiveGraphEditor->GetSelectedNodes())
		{
			UEdGraphNode* GraphNode = Cast<UEdGraphNode>(Node);
			if (GraphNode == nullptr)
				continue;

			switch (EPhraseDirectionalInput(Direction->GetValue()))
			{
			case EPhraseDirectionalInput::UP:
				GraphNode->NodePosY -= Amount->GetValue();
				break;

			case EPhraseDirectionalInput::DOWN:
				GraphNode->NodePosY += Amount->GetValue();
				break;

			case EPhraseDirectionalInput::LEFT:
				GraphNode->NodePosX -= Amount->GetValue();
				break;

			case EPhraseDirectionalInput::RIGHT:
				GraphNode->NodePosX += Amount->GetValue();
				break;
			}
		}
	});

	TSharedPtr<FPhraseEventNode> NodeSelectionComment = MakeShared<FPhraseEventNode>();
	NodeSelectionComment->OnPhraseParsed.BindLambda([this](FParseRecord& Record) {
		GET_ACTIVE_TAB(ActiveGraphEditor, SGraphEditor);

		UEdGraph* Graph = ActiveGraphEditor->GetCurrentGraph();

		TSharedPtr<FEdGraphSchemaAction> CommentCreateAction = Graph->GetSchema()->GetCreateCommentAction();
		if (CommentCreateAction.IsValid())
		{
			CommentCreateAction->PerformAction(Graph, nullptr, FVector2D(0, 0), true);
		}
	});

	TSharedPtr<FPhraseEventNode> NodeSelectionAlignment = MakeShared<FPhraseEventNode>();
	NodeSelectionAlignment->OnPhraseParsed.BindLambda([this](FParseRecord& Record) {
		GET_ACTIVE_TAB(ActiveGraphEditor, SGraphEditor);

		UParseEnumInput* PositionInput = Record.GetPhraseInput<UParseEnumInput>(TEXT("POSITION"));
		if (PositionInput == nullptr)
			return;

		for (UObject* Node : ActiveGraphEditor->GetSelectedNodes())
		{
			UEdGraphNode* GraphNode = Cast<UEdGraphNode>(Node);
			if (GraphNode == nullptr)
				continue;

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

				case EPhrasePositionalInput::CENTER:
					ActiveGraphEditor->OnAlignCenter();
					break;

				case EPhrasePositionalInput::RIGHT:
					ActiveGraphEditor->OnAlignRight();
					break;
			}
		}
		});

	TSharedPtr<FPhraseEventNode> NodeSelectionStraighten = MakeShared<FPhraseEventNode>();
	NodeSelectionStraighten->OnPhraseParsed.BindLambda([this](FParseRecord& Record) {
		GET_ACTIVE_TAB(ActiveGraphEditor, SGraphEditor);

		ActiveGraphEditor->StraightenConnections();
	});

	// ------



	// Node Menu Events
	TDelegate<TSharedPtr<IMenu>(FParseRecord&)> GetAddNodeMenuEvent;
	GetAddNodeMenuEvent.BindLambda(
		[this] (FParseRecord& Record) -> TSharedPtr<IMenu> {

			TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
			if (!ActiveTab.IsValid())
			{
				return TSharedPtr<IMenu>();
			}

			TSharedPtr<SGraphEditor> ActiveGraphEditor = StaticCastSharedPtr<SGraphEditor>(ActiveTab->GetContent().ToSharedPtr());
			if (!ActiveGraphEditor.IsValid())
			{
				return TSharedPtr<IMenu>();
			}

			SGraphPanel* GraphPanel = ActiveGraphEditor->GetGraphPanel();

			FDisplayMetrics DisplayMetrics;
			FSlateApplication::Get().GetDisplayMetrics(DisplayMetrics);

			DisplayMetrics.PrimaryDisplayHeight;

			TSharedPtr<SWindow> TopLevelWindow = FSlateApplication::Get().GetActiveTopLevelRegularWindow();

			FVector2D SpawnLocation;
			if (TopLevelWindow.IsValid())
			{
				SpawnLocation = TopLevelWindow->GetPositionInScreen();

				SpawnLocation.X += TopLevelWindow->GetSizeInScreen().X / 5;
				SpawnLocation.Y += TopLevelWindow->GetSizeInScreen().Y / 5;
			}
			else
			{
				SpawnLocation = FVector2D(DisplayMetrics.PrimaryDisplayWidth / 4, DisplayMetrics.PrimaryDisplayHeight / 4);
			}

			TSharedPtr<SWidget> ContextWidgetToFocus = GraphPanel->SummonContextMenu(
				SpawnLocation, 
				GraphPanel->GetPastePosition(), 
				nullptr, 
				nullptr, 
				TArray<UEdGraphPin*>()
			);

			if (ContextWidgetToFocus.IsValid())
			{
				FSlateApplication::Get().SetKeyboardFocus(ContextWidgetToFocus);
			}

			TSharedPtr<SWidget> KeyboardFocusedWidget = FSlateApplication::Get().GetKeyboardFocusedWidget();
			if (!KeyboardFocusedWidget.IsValid())
			{
				return TSharedPtr<IMenu>();
			}

			FWidgetPath KeyboardFocusWidgetPath;
			if (FSlateApplication::Get().FindPathToWidget(KeyboardFocusedWidget.ToSharedRef(), KeyboardFocusWidgetPath))
			{
				TSharedPtr<IMenu> Menu = FSlateApplication::Get().FindMenuInWidgetPath(KeyboardFocusWidgetPath);

				return Menu;
			}
			
			return TSharedPtr<IMenu>();
		}
	);

	TDelegate<TSharedPtr<IMenu>(FParseRecord&)> GetAddNodeMenu_PinEvent;
	GetAddNodeMenu_PinEvent.BindLambda(
		[this](FParseRecord& Record) -> TSharedPtr<IMenu> {

			TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
			if (!ActiveTab.IsValid())
			{
				return TSharedPtr<IMenu>();
			}

			TSharedPtr<SGraphEditor> ActiveGraphEditor = StaticCastSharedPtr<SGraphEditor>(ActiveTab->GetContent().ToSharedPtr());
			if (!ActiveGraphEditor.IsValid())
			{
				return TSharedPtr<IMenu>();
			}

			SGraphPanel* GraphPanel = ActiveGraphEditor->GetGraphPanel();

			FDisplayMetrics DisplayMetrics;
			FSlateApplication::Get().GetDisplayMetrics(DisplayMetrics);

			DisplayMetrics.PrimaryDisplayHeight;

			TSharedPtr<SWindow> TopLevelWindow = FSlateApplication::Get().GetActiveTopLevelRegularWindow();

			FVector2D SpawnLocation;
			if (TopLevelWindow.IsValid())
			{
				SpawnLocation = TopLevelWindow->GetPositionInScreen();

				SpawnLocation.X += TopLevelWindow->GetSizeInScreen().X / 5;
				SpawnLocation.Y += TopLevelWindow->GetSizeInScreen().Y / 5;
			}
			else
			{
				SpawnLocation = FVector2D(DisplayMetrics.PrimaryDisplayWidth / 4, DisplayMetrics.PrimaryDisplayHeight / 4);
			}

			TSharedPtr<FGraphIndexer> IndexerForGraph = AssetAccessibilityRegistry->GetGraphIndexer(ActiveGraphEditor->GetCurrentGraph());

			UParseIntInput* NodeIndexInput = Record.GetPhraseInput<UParseIntInput>(TEXT("NODE_INDEX"));
			UParseIntInput* PinIndexInput = Record.GetPhraseInput<UParseIntInput>(TEXT("PIN_INDEX"));

			if (NodeIndexInput == nullptr || PinIndexInput == nullptr)
			{
				return TSharedPtr<IMenu>();
			}

			TSharedPtr<SWidget> ContextWidgetToFocus = GraphPanel->SummonContextMenu(
				SpawnLocation,
				GraphPanel->GetPastePosition(),
				nullptr,
				nullptr,
				TArray<UEdGraphPin*> {
					IndexerForGraph->GetPin(
						NodeIndexInput->GetValue(),
						PinIndexInput->GetValue()
					)
				}
			);

			if (ContextWidgetToFocus.IsValid())
			{
				FSlateApplication::Get().SetKeyboardFocus(ContextWidgetToFocus);
			}

			TSharedPtr<SWidget> KeyboardFocusedWidget = FSlateApplication::Get().GetKeyboardFocusedWidget();
			if (!KeyboardFocusedWidget.IsValid())
			{
				return TSharedPtr<IMenu>();
			}

			FWidgetPath KeyboardFocusWidgetPath;
			if (FSlateApplication::Get().FindPathToWidget(KeyboardFocusedWidget.ToSharedRef(), KeyboardFocusWidgetPath))
			{
				TSharedPtr<IMenu> Menu = FSlateApplication::Get().FindMenuInWidgetPath(KeyboardFocusWidgetPath);

				return Menu;
			}

			return TSharedPtr<IMenu>();
		}
	);

	// -----

	// Node Add Context

	TSharedPtr<FPhraseEventNode> Context_SelectAction = MakeShared<FPhraseEventNode>();
	Context_SelectAction->OnPhraseParsed.BindLambda(
		[this](FParseRecord& Record) {
			UAccessibilityAddNodeContextMenu* ContextMenu = Record.GetContextObj<UAccessibilityAddNodeContextMenu>();
			if (ContextMenu == nullptr)
			{
				UE_LOG(LogOpenAccessibility, Log, TEXT("ContextMenu Cannot Be Obtained"));

				return;
			}

			UParseIntInput* Index = Record.GetPhraseInput<UParseIntInput>(TEXT("NODE_INDEX"));

			ContextMenu->PerformGraphAction(Index->GetValue());
		}
	);

	TSharedPtr<FPhraseEventNode> Context_SearchNewPhrase = MakeShared<FPhraseEventNode>();
	Context_SearchNewPhrase->OnPhraseParsed.BindLambda(
		[this](FParseRecord& Record) {
			UAccessibilityAddNodeContextMenu* ContextMenu = Record.GetContextObj<UAccessibilityAddNodeContextMenu>();
			if (ContextMenu == nullptr)
			{
				UE_LOG(LogOpenAccessibility, Log, TEXT("ContextMenu Cannot Be Obtained"));

				return;
			}
			
			UParseStringInput* Phrase = Record.GetPhraseInput<UParseStringInput>(TEXT("SEARCH_PHRASE"));

			ContextMenu->SetFilterText(Phrase->GetValue());
		}
	);

	TSharedPtr<FPhraseEventNode> Context_SearchAppendPhrase = MakeShared<FPhraseEventNode>();
	Context_SearchAppendPhrase->OnPhraseParsed.BindLambda(
		[this](FParseRecord& Record) {
			UAccessibilityAddNodeContextMenu* ContextMenu = Record.GetContextObj<UAccessibilityAddNodeContextMenu>();
			if (ContextMenu == nullptr)
			{
				UE_LOG(LogOpenAccessibility, Log, TEXT("ContextMenu Cannot Be Obtained"));

				return;
			}

			UParseStringInput* Phrase = Record.GetPhraseInput<UParseStringInput>(TEXT("SEARCH_PHRASE"));

			ContextMenu->AppendFilterText(Phrase->GetValue());
		}
	);

	TSharedPtr<FPhraseEventNode> Context_SearchReset = MakeShared<FPhraseEventNode>();
	Context_SearchReset->OnPhraseParsed.BindLambda(
		[this](FParseRecord& Record) {
			UAccessibilityAddNodeContextMenu* ContextMenu = Record.GetContextObj<UAccessibilityAddNodeContextMenu>();
			if (ContextMenu == nullptr)
			{
				UE_LOG(LogOpenAccessibility, Log, TEXT("ContextMenu Cannot Be Obtained"));

				return;
			}

			ContextMenu->ResetFilterText();
		}
	);

	TSharedPtr<FPhraseEventNode> Context_Scroll = MakeShared<FPhraseEventNode>();
	Context_Scroll->OnPhraseParsed.BindLambda(
		[this](FParseRecord& Record) {
			UAccessibilityAddNodeContextMenu* ContextMenu = Record.GetContextObj<UAccessibilityAddNodeContextMenu>();
			if (ContextMenu == nullptr)
			{
				UE_LOG(LogOpenAccessibility, Log, TEXT("ContextMenu Cannot Be Obtained"));

				return;
			}

			UParseEnumInput* Direction = Record.GetPhraseInput<UParseEnumInput>(TEXT("DIRECTION"));
			UParseIntInput* Amount = Record.GetPhraseInput<UParseIntInput>(TEXT("AMOUNT"));
			
			switch (EPhraseScrollInput(Direction->GetValue()))
			{
				case EPhraseScrollInput::UP:
					ContextMenu->AppendScrollDistance(-Amount->GetValue());
					break;

				case EPhraseScrollInput::DOWN:
					ContextMenu->AppendScrollDistance(Amount->GetValue());
					break;

				case EPhraseScrollInput::TOP:
					ContextMenu->SetScrollDistanceTop();
					break;

				case EPhraseScrollInput::BOTTOM:
					ContextMenu->SetScrollDistanceBottom();
					break;
			}
		}
	);

	TSharedPtr<FPhraseEventNode> Context_ToggleContextAwareness = MakeShared<FPhraseEventNode>();
	Context_ToggleContextAwareness->OnPhraseParsed.BindLambda(
		[this](FParseRecord& Record) {
			UAccessibilityAddNodeContextMenu* ContextMenu = Record.GetContextObj<UAccessibilityAddNodeContextMenu>();
			if (ContextMenu == nullptr)
			{
				UE_LOG(LogOpenAccessibility, Log, TEXT("ContextMenu Cannot Be Obtained"));
				return;
			}

			ContextMenu->ToggleContextAwareness();
		}
	);

	TPhraseNodeArray AddNodeContextChildren = TPhraseNodeArray{

				MakeShared<FPhraseNode>(TEXT("SELECT"),
				TPhraseNodeArray {

						MakeShared<FPhraseInputNode<int32>>(TEXT("NODE_INDEX"),
						TPhraseNodeArray {

								Context_SelectAction
						})
				}),

				MakeShared<FPhraseNode>(TEXT("SEARCH"),
				TPhraseNodeArray {

						MakeShared<FPhraseNode>(TEXT("NEW"),
						TPhraseNodeArray {

								MakeShared<FPhraseStringInputNode>(TEXT("SEARCH_PHRASE"),
								TPhraseNodeArray {
									Context_SearchNewPhrase
								})
						}),

						MakeShared<FPhraseNode>(TEXT("ADD"),
						TPhraseNodeArray {

								MakeShared<FPhraseStringInputNode>(TEXT("SEARCH_PHRASE"),
								TPhraseNodeArray {
									Context_SearchAppendPhrase
								})
						}),

						MakeShared<FPhraseNode>(TEXT("RESET"),
						TPhraseNodeArray {
							Context_SearchReset
						})
				}),

				MakeShared<FPhraseNode>(TEXT("SCROLL"),
				TPhraseNodeArray {

						MakeShared<FPhraseScrollInputNode>(TEXT("DIRECTION"),
						TPhraseNodeArray {

								MakeShared<FPhraseInputNode<int32>>(TEXT("AMOUNT"),
								TPhraseNodeArray {
									Context_Scroll
								})
						})
				}),

				MakeShared<FPhraseNode>(TEXT("TOGGLE"),
				TPhraseNodeArray {
						Context_ToggleContextAwareness
				})
	};

	TSharedPtr<FPhraseContextMenuNode<UAccessibilityAddNodeContextMenu>> AddNodeContextMenu = MakeShared<FPhraseContextMenuNode<UAccessibilityAddNodeContextMenu>>(
		TEXT("ADD"),
		1.5f,
		GetAddNodeMenuEvent,
		AddNodeContextChildren
	);

	TSharedPtr<FPhraseContextMenuNode<UAccessibilityAddNodeContextMenu>> AddNodeContextMenu_PinEvent = MakeShared<FPhraseContextMenuNode<UAccessibilityAddNodeContextMenu>>(
		TEXT("ADD"),
		1.5f,
		GetAddNodeMenu_PinEvent,
		AddNodeContextChildren
	);
	// -----

	FOpenAccessibilityCommunicationModule::Get().PhraseTree->BindBranch(
		MakeShared<FPhraseNode>(
			TEXT("NODE"),
			TPhraseNodeArray {

				MakeShared<FPhraseInputNode<int32>>(TEXT("NODE_INDEX"),
				TPhraseNodeArray {

					MakeShared<FPhraseNode>(TEXT("MOVE"),
					TPhraseNodeArray {

						MakeShared<FPhrase2DDirectionalInputNode>(TEXT("DIRECTION"),
						TPhraseNodeArray {

							MakeShared<FPhraseInputNode<int32>>(TEXT("AMOUNT"),
							TPhraseNodeArray {
								MoveEventNode
							})
						})
					}),

					MakeShared<FPhraseNode>(TEXT("DELETE"),
					TPhraseNodeArray {
						DeleteEventNode
					}),

					MakeShared<FPhraseNode>(TEXT("PIN"),
					TPhraseNodeArray {

						MakeShared<FPhraseInputNode<int32>>(TEXT("PIN_INDEX"),
						TPhraseNodeArray {

							MakeShared<FPhraseNode>(TEXT("CONNECT"),
							TPhraseNodeArray {

								MakeShared<FPhraseNode>(TEXT("NODE"),
								TPhraseNodeArray {

									AddNodeContextMenu_PinEvent,

									MakeShared<FPhraseInputNode<int32>>(TEXT("NODE_INDEX"),
									TPhraseNodeArray {

										MakeShared<FPhraseNode>(TEXT("PIN"),
										TPhraseNodeArray {

											MakeShared<FPhraseInputNode<int32>>(TEXT("PIN_INDEX"),
											TPhraseNodeArray {
												PinConnectEventNode
											})

										})

									}, NodeIndexFocusEvent)
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
												PinDisconnectEventNode
											})
										})

									}, NodeIndexFocusEvent)
								})
							})
						})
					}),

				}, NodeIndexFocusEvent),
				
				MakeShared<FPhraseNode>(TEXT("SELECT"),
				TPhraseNodeArray {

					MakeShared<FPhraseNode>(TEXT("ADD"),
					TPhraseNodeArray {
						
						MakeShared<FPhraseInputNode<int32>>(TEXT("NODE_INDEX"),
						TPhraseNodeArray {
							NodeSelectionAddNode
						})

					}),

					MakeShared<FPhraseNode>(TEXT("REMOVE"),
					TPhraseNodeArray {
						
						MakeShared<FPhraseInputNode<int32>>(TEXT("NODE_INDEX"),
						TPhraseNodeArray {
							NodeSelectionRemoveNode
						})

					}),

					MakeShared<FPhraseNode>(TEXT("RESET"),
					TPhraseNodeArray {
						NodeSelectionReset
					}),

					MakeShared<FPhraseNode>(TEXT("STRAIGHTEN"),
					TPhraseNodeArray {
						NodeSelectionStraighten
					}),

					MakeShared<FPhraseNode>(TEXT("MOVE"),
					TPhraseNodeArray {
						
						MakeShared<FPhrase2DDirectionalInputNode>(TEXT("DIRECTION"),
						TPhraseNodeArray {

							MakeShared<FPhraseInputNode<int32>>(TEXT("AMOUNT"),
							TPhraseNodeArray {
								NodeSelectionMove
							})

						})

					}),

					MakeShared<FPhraseNode>(TEXT("COMMENT"), 
					TPhraseNodeArray {
						NodeSelectionComment
					}),

					MakeShared<FPhraseNode>(TEXT("ALIGNMENT"),
					TPhraseNodeArray {

						MakeShared<FPhrasePositionalInputNode>(TEXT("POSITION"),
						TPhraseNodeArray {
							NodeSelectionAlignment
						})

					}),

				}),

				AddNodeContextMenu,
			})
	);
}

void FOpenAccessibilityModule::CreateTranscriptionVisualization()
{
	TranscriptionVisualizer = MakeShared<FTranscriptionVisualizer, ESPMode::ThreadSafe>();

	FOpenAccessibilityCommunicationModule::Get().OnTranscriptionRecieved
		.AddSP(TranscriptionVisualizer.ToSharedRef(), &FTranscriptionVisualizer::OnTranscriptionRecieved);
}

void FOpenAccessibilityModule::RegisterConsoleCommands()
{
	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("OpenAccessibility.Debug.FlashActiveTab"),
		TEXT("Flashes the active tab in the editor."),

		FConsoleCommandDelegate::CreateLambda([]() {
			UE_LOG(LogOpenAccessibility, Display, TEXT("OpenAccessibility.Debug.FlashActiveTab"));

			TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
			if (!ActiveTab.IsValid())
			{
				return;
			}
			
			ActiveTab->FlashTab();
			
			UE_LOG(LogOpenAccessibility, Log, TEXT("Active Tab Content Type: %s"), *ActiveTab->GetContent()->GetTypeAsString())

		}),

		ECVF_Default
	));

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("OpenAccessibility.Debug.LogActiveIndexes"),
		TEXT("Logs the Active Indexes of the Active Tab"),

		FConsoleCommandDelegate::CreateLambda([this]() {

			TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
			SGraphEditor* ActiveGraphEditor = (SGraphEditor*)ActiveTab->GetContent().ToSharedPtr().Get();
			if (ActiveGraphEditor == nullptr)
			{
				UE_LOG(LogOpenAccessibility, Display, TEXT("Active Tab Not SGraphEditor"));
				return;
			}


			TSharedRef<FGraphIndexer> GraphIndexer = AssetAccessibilityRegistry->GetGraphIndexer(ActiveGraphEditor->GetCurrentGraph());
		}),

		ECVF_Default
	));

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("OpenAccessibility.Debug.OpenAccessibilityGraph_AddNodeMenu"),
		TEXT("Opens the context menu for adding nodes for the active graph editor."),

		FConsoleCommandDelegate::CreateLambda(
			[this]() {

				TSharedPtr<SGraphEditor> ActiveGraphEditor = nullptr;
				{
					// Getting Graph Editor Section

					TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
					if (!ActiveTab.IsValid())
						return;

					ActiveGraphEditor = StaticCastSharedPtr<SGraphEditor>(ActiveTab->GetContent().ToSharedPtr());
					if (!ActiveGraphEditor.IsValid())
					{
						UE_LOG(LogOpenAccessibility, Display, TEXT("Active Tab Not SGraphEditor"));
						return;
					}
				}

				TSharedPtr<IMenu> Menu;
				TSharedPtr<SWindow> MenuWindow;
				TSharedPtr<SGraphActionMenu> GraphActionMenu;
				TSharedPtr<SSearchBox> SearchBox;
				TSharedPtr<STreeView<TSharedPtr<FGraphActionNode>>> TreeView;
				{
					// Summoning Create Node Menu Section
					// and Getting any Key Widgets

					ActiveGraphEditor->GetGraphPanel()->SummonCreateNodeMenuFromUICommand(0);

					TSharedPtr<SWidget> KeyboardFocusedWidget = StaticCastSharedPtr<SEditableText>(FSlateApplication::Get().GetKeyboardFocusedWidget());
					if (!KeyboardFocusedWidget.IsValid())
					{
						UE_LOG(LogOpenAccessibility, Display, TEXT("Cannot get Keyboard Focused Widget."));
						return;
					}

					UE_LOG(LogOpenAccessibility, Display, TEXT("Keyboard Focused Widget Type: %s"), *KeyboardFocusedWidget->GetTypeAsString());

					// Getting Menu Object
					FWidgetPath KeyboardFocusedWidgetPath;
					if (FSlateApplication::Get().FindPathToWidget(KeyboardFocusedWidget.ToSharedRef(), KeyboardFocusedWidgetPath))
					{
						UE_LOG(LogOpenAccessibility, Display, TEXT("Keyboard Focused Widget Path Found."));
					}
					else return;

					Menu = FSlateApplication::Get().FindMenuInWidgetPath(KeyboardFocusedWidgetPath);

					// Getting Graph Action Menu Object
					GraphActionMenu = StaticCastSharedPtr<SGraphActionMenu>(
						KeyboardFocusedWidget
							->GetParentWidget()
							->GetParentWidget()
							->GetParentWidget()
							->GetParentWidget()
							->GetParentWidget()
					);

					SearchBox = StaticCastSharedPtr<SSearchBox>(
						KeyboardFocusedWidget
							->GetParentWidget()
							->GetParentWidget()
							->GetParentWidget()
					);

					TSharedRef<SWidget> SearchBoxSibling = SearchBox->GetParentWidget()->GetChildren()->GetChildAt(1);
					TreeView = StaticCastSharedRef<STreeView<TSharedPtr<FGraphActionNode>>>(
						SearchBoxSibling->GetChildren()->GetChildAt(0)->GetChildren()->GetChildAt(0)
					);

					MenuWindow = FSlateApplication::Get().FindWidgetWindow(KeyboardFocusedWidget.ToSharedRef());
				}
				
				UAccessibilityAddNodeContextMenu* MenuWrapper = NewObject<UAccessibilityAddNodeContextMenu>();
				MenuWrapper->AddToRoot();
				MenuWrapper->Init(
					Menu.ToSharedRef(),
					FOpenAccessibilityCommunicationModule::Get().PhraseTree->AsShared()
				);

				MenuWrapper->ScaleMenu(1.5f);
			}),

		ECVF_Default
	));
}

void FOpenAccessibilityModule::UnregisterConsoleCommands()
{
	IConsoleCommand* ConsoleCommand = nullptr;
	while (ConsoleCommands.Num() > 0)
	{
		ConsoleCommand = ConsoleCommands.Pop();

		IConsoleManager::Get().UnregisterConsoleObject(ConsoleCommand);

		delete ConsoleCommand;
		ConsoleCommand = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOpenAccessibilityModule, OpenAccessibility)