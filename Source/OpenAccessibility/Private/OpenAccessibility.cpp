// Copyright Epic Games, Inc. All Rights Reserved.

#include "OpenAccessibility.h"
#include "OpenAccessibilityCommunication.h"
#include "OpenAccessibilityLogging.h"

#include "PhraseTree/PhraseNode.h"
#include "PhraseTree/PhraseInputNode.h"
#include "PhraseTree/PhraseDirectionalInputNode.h"
#include "PhraseTree/PhraseEventNode.h"

#include "PhraseTree/Containers/Input/UParseInput.h"
#include "PhraseTree/Containers/Input/UParseIntInput.h"
#include "PhraseTree/Containers/Input/UParseStringInput.h"
#include "PhraseTree/Containers/Input/UParseEnumInput.h"

#include "GraphActionNode.h"
#include "SGraphPanel.h"
#include "Widgets/Input/SSearchBox.h"

#include "Framework/Docking/TabManager.h"
#include "Logging/StructuredLog.h"

#define LOCTEXT_NAMESPACE "FOpenAccessibilityModule"

void FOpenAccessibilityModule::StartupModule()
{
	UE_LOG(LogOpenAccessibility, Display, TEXT("OpenAccessibilityModule::StartupModule()"));

	// Create the Asset Registry
	AssetAccessibilityRegistry = MakeShared<FAssetAccessibilityRegistry, ESPMode::ThreadSafe>();

	// Register the Accessibility Node Factory
	AccessibilityNodeFactory = MakeShared<FAccessibilityNodeFactory, ESPMode::ThreadSafe>();
	FEdGraphUtilities::RegisterVisualNodeFactory(AccessibilityNodeFactory);

	// Register Console Commands
	RegisterConsoleCommands();

	BindGraphInteractionBranch();
	BindLocalLocomotionBranch();
}

void FOpenAccessibilityModule::ShutdownModule()
{
	UE_LOG(LogOpenAccessibility, Display, TEXT("OpenAccessibilityModule::ShutdownModule()"));

	UnregisterConsoleCommands();
}

void FOpenAccessibilityModule::BindLocalLocomotionBranch()
{
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

	FOpenAccessibilityCommunicationModule::Get().PhraseTree->BindBranch(
		MakeShared<FPhraseNode>(TEXT("VIEW"),
		TPhraseNodeArray{
			
			MakeShared<FPhraseNode>(TEXT("MOVE"),
			TPhraseNodeArray {
				
				MakeShared<FPhrase2DDirectionalInputNode>(TEXT("DIRECTION"),
				TPhraseNodeArray {
					
					MakeShared<FPhraseInputNode>(TEXT("AMOUNT"),
					TPhraseNodeArray {
						MoveViewportEventNode
					})
				})
			}),

			MakeShared<FPhraseNode>(TEXT("ZOOM"),
			TPhraseNodeArray {
				
				MakeShared<FPhrase2DDirectionalInputNode>(TEXT("DIRECTION"),
				TPhraseNodeArray {
					
					MakeShared<FPhraseInputNode>(TEXT("AMOUNT"),
					TPhraseNodeArray {
						ZoomViewportEventNode
					})
				})
			}),

			MakeShared<FPhraseNode>(TEXT("FOCUS"),
			TPhraseNodeArray {
				
				MakeShared<FPhraseInputNode>(TEXT("INDEX"),
				TPhraseNodeArray {
					IndexFocusEventNode
				})
			})
		})
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

			switch (EPhraseDirectionalInput(MoveDirection->GetValue()))
			{
			case EPhraseDirectionalInput::UP:
				GraphNode->NodePosY -= MoveAmount->GetValue();
				break;

			case EPhraseDirectionalInput::DOWN:
				GraphNode->NodePosY += MoveAmount->GetValue();
				break;

			case EPhraseDirectionalInput::LEFT:
				GraphNode->NodePosX -= MoveAmount->GetValue();
				break;

			case EPhraseDirectionalInput::RIGHT:
				GraphNode->NodePosX += MoveAmount->GetValue();
				break;

			default:
				UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- Invalid Direction --"));
				return;
			}
		}
		else
		{
			UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- Active Tab Not SGraphEditor --"));
			return;
		}
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
		TArray<UParseIntInput*> NodeInputs;
		Record.GetPhraseInputs<UParseIntInput>(TEXT("NODE_INDEX"), NodeInputs);

		TArray<UParseIntInput*> PinInputs;
		Record.GetPhraseInputs<UParseIntInput>(TEXT("PIN_INDEX"), PinInputs);

		if (NodeInputs.Num() < 2 || PinInputs.Num() < 2)
		{
			UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- Invalid Inputs Length --"));
			return;
		}

		UEdGraphPin* SourcePin = IndexerForGraph->GetPin(
			NodeInputs[0]->GetValue(),
			PinInputs[0]->GetValue()
		);

		UEdGraphPin* TargetPin = IndexerForGraph->GetPin(
			NodeInputs[1]->GetValue(),
			PinInputs[1]->GetValue()
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
		TArray<UParseIntInput*> NodeInputs = Record.GetPhraseInputs<UParseIntInput>(TEXT("NODE_INDEX"));

		TArray<UParseIntInput*> PinInputs = Record.GetPhraseInputs<UParseIntInput>(TEXT("PIN_INDEX"));


		if (NodeInputs.Num() < 2 || PinInputs.Num() < 2)
		{
			UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- Invalid Inputs Length --"));
			return;
		}

		UEdGraphPin* SourcePin = IndexerForGraph->GetPin(
			NodeInputs[0]->GetValue(),
			PinInputs[0]->GetValue()
		);

		UEdGraphPin* TargetPin = IndexerForGraph->GetPin(
			NodeInputs[1]->GetValue(),
			PinInputs[1]->GetValue()
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


	// Node Menu Events
	TDelegate<void(const FParseRecord& Record)> OpenAddNodeMenuEvent;
	OpenAddNodeMenuEvent.BindLambda(
		[this](FParseRecord& Record) {
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
			SGraphPanel* GraphPanel = ActiveGraphEditor->GetGraphPanel();
			
			GraphPanel->SummonCreateNodeMenuFromUICommand(0);

			TSharedPtr<SWidget> KeyboardFocusedWidget = StaticCastSharedPtr<SEditableText>(FSlateApplication::Get().GetKeyboardFocusedWidget());
			
			
			if (KeyboardFocusedWidget.IsValid()) 
			{	
				FWidgetPath KeyboardFocusWidgetPath;
				if (FSlateApplication::Get().FindPathToWidget(KeyboardFocusedWidget.ToSharedRef(), KeyboardFocusWidgetPath))
				{
					TSharedPtr<IMenu> Menu = FSlateApplication::Get().FindMenuInWidgetPath(KeyboardFocusWidgetPath);

					UAccessibilityAddNodeContextMenu* MenuWrapper = NewObject<UAccessibilityAddNodeContextMenu>();
					MenuWrapper->AddToRoot();
					MenuWrapper->Init(
						Menu.ToSharedRef()
					);

					MenuWrapper->ScaleMenu(1.5f);

					Record.SetContextObj(MenuWrapper);
				}
			}
		}
	);
	// -----

	// Node Add Context Events
	TSharedPtr<FPhraseEventNode> Context_SelectAction = MakeShared<FPhraseEventNode>();
	Context_SelectAction->OnPhraseParsed.BindLambda(
		[this](FParseRecord& Record) {
			UAccessibilityAddNodeContextMenu* ContextMenu = const_cast<FParseRecord&>(Record).GetContextObj<UAccessibilityAddNodeContextMenu>();
			if (ContextMenu == nullptr)
			{
				UE_LOG(LogOpenAccessibility, Log, TEXT("ContextMenu Cannot Be Obtained"));

				return;
			}

			UParseIntInput* Index = Record.GetPhraseInput<UParseIntInput>(TEXT("NODE_INDEX"));

			ContextMenu->PerformGraphAction(Index->GetValue());
		}
	);

	TSharedPtr<FPhraseEventNode> Context_SearchPhrase = MakeShared<FPhraseEventNode>();
	Context_SearchPhrase->OnPhraseParsed.BindLambda(
		[this](const FParseRecord& Record) {
			UAccessibilityAddNodeContextMenu* ContextMenu = const_cast<FParseRecord&>(Record).GetContextObj<UAccessibilityAddNodeContextMenu>();
			if (ContextMenu == nullptr)
			{
				UE_LOG(LogOpenAccessibility, Log, TEXT("ContextMenu Cannot Be Obtained"));

				return;
			}
			
			// Figure Out Phrase Input.
			UE_LOG(LogOpenAccessibility, Display, TEXT("NOT IMPLEMENTED YET, DUE TO REQUIREMENT OF PHRASE STRING PARSING INTO INPUT"));
		}
	);

	TSharedPtr<FPhraseEventNode> Context_SearchReset = MakeShared<FPhraseEventNode>();
	Context_SearchPhrase->OnPhraseParsed.BindLambda(
		[this](const FParseRecord& Record) {
			UAccessibilityAddNodeContextMenu* ContextMenu = const_cast<FParseRecord&>(Record).GetContextObj<UAccessibilityAddNodeContextMenu>();
			if (ContextMenu == nullptr)
			{
				UE_LOG(LogOpenAccessibility, Log, TEXT("ContextMenu Cannot Be Obtained"));

				return;
			}

			ContextMenu->ResetFilterText();
		}
	);

	TSharedPtr<FPhraseEventNode> Context_Exit = MakeShared<FPhraseEventNode>();
	Context_Exit->OnPhraseParsed.BindLambda(
		[this](const FParseRecord& Record) {
			UAccessibilityAddNodeContextMenu* ContextMenu = const_cast<FParseRecord&>(Record).GetContextObj<UAccessibilityAddNodeContextMenu>();
			if (ContextMenu == nullptr)
			{
				UE_LOG(LogOpenAccessibility, Log, TEXT("ContextMenu Cannot Be Obtained"));

				return;
			}

			ContextMenu->Close();
		}
	);

	FOpenAccessibilityCommunicationModule::Get().PhraseTree->BindBranch(
		MakeShared<FPhraseNode>(
			TEXT("NODE"),
			TPhraseNodeArray{

				MakeShared<FPhraseInputNode>(TEXT("NODE_INDEX"),
				TPhraseNodeArray {

							MakeShared<FPhraseNode>(TEXT("MOVE"),
							TPhraseNodeArray {

									MakeShared<FPhrase2DDirectionalInputNode>(TEXT("DIRECTION"),
									TPhraseNodeArray {

											MakeShared<FPhraseInputNode>(TEXT("AMOUNT"),
											TPhraseNodeArray {
												MoveEventNode
											})
									})
							}),

							MakeShared<FPhraseNode>(TEXT("PIN"),
							TPhraseNodeArray {

									MakeShared<FPhraseInputNode>(TEXT("PIN_INDEX"),
									TPhraseNodeArray {

											MakeShared<FPhraseNode>(TEXT("CONNECT"),
											TPhraseNodeArray {

													MakeShared<FPhraseInputNode>(TEXT("NODE_INDEX"),
													TPhraseNodeArray {

															MakeShared<FPhraseInputNode>(TEXT("PIN_INDEX"),
															TPhraseNodeArray {
																PinConnectEventNode
															})
													})
											}),

											MakeShared<FPhraseNode>(TEXT("DISCONNECT"),
											TPhraseNodeArray {

													MakeShared<FPhraseInputNode>(TEXT("NODE_INDEX"),
													TPhraseNodeArray {

															MakeShared<FPhraseInputNode>(TEXT("PIN_INDEX"),
															TPhraseNodeArray {
																PinDisconnectEventNode
															})
													}, NodeIndexFocusEvent)
											})
									})
							}),
					}, NodeIndexFocusEvent),

				MakeShared<FPhraseNode>(
				TEXT("ADD"),
				OpenAddNodeMenuEvent,
				TPhraseNodeArray{

					MakeShared<FPhraseNode>(TEXT("SELECT"),
					TPhraseNodeArray {

							MakeShared<FPhraseInputNode>(TEXT("NODE_INDEX"),
							TPhraseNodeArray {
										
									Context_SelectAction
							})
					}),

					MakeShared<FPhraseNode>(TEXT("SEARCH"),
					TPhraseNodeArray {

						MakeShared<FPhraseNode>(TEXT("ADD"),
						TPhraseNodeArray {
							Context_SearchPhrase
						}),

						MakeShared<FPhraseNode>(TEXT("RESET"),
						TPhraseNodeArray {
							Context_SearchReset
						})
					})

				})
			})
	);
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

				SGraphEditor* ActiveGraphEditor = nullptr;
				{
					// Getting Graph Editor Section

					TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
					if (!ActiveTab.IsValid())
						return;

					ActiveGraphEditor = (SGraphEditor*) ActiveTab->GetContent().ToSharedPtr().Get();
					if (ActiveGraphEditor == nullptr)
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
					GraphActionMenu.ToSharedRef(),
					TreeView.ToSharedRef()
				);

				MenuWrapper->ScaleMenu(1.5f);
				 
				UObject* ContextMenu = MenuWrapper;
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