// Copyright Epic Games, Inc. All Rights Reserved.

#include "OpenAccessibility.h"
#include "OpenAccessibilityCommunication.h"
#include "OpenAccessibilityLogging.h"

#include "PhraseTree/PhraseNode.h"
#include "PhraseTree/PhraseInputNode.h"
#include "PhraseTree/PhraseDirectionalInputNode.h"
#include "PhraseTree/PhraseEventNode.h"

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

	// Bind Branch to Phrase Tree
	TSharedPtr<FPhraseEventNode> MoveEventNode = MakeShared<FPhraseEventNode>();
	MoveEventNode->OnPhraseParsed.BindLambda([this](const FParseRecord& Record) {
		TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
		if (!ActiveTab.IsValid())
		{
			return;
		}

		SGraphEditor* ActiveGraphEditor = (SGraphEditor*)ActiveTab->GetContent().ToSharedPtr().Get();

		if (ActiveGraphEditor != nullptr)
		{
			// Get Inputs
			const int NodeIndex = *Record.PhraseInputs.Find("NODE_INDEX");
			const int MoveDirection = *Record.PhraseInputs.Find("DIRECTION");
			const int MoveAmount = *Record.PhraseInputs.Find("AMOUNT");

			UEdGraphNode* GraphNode = AssetAccessibilityRegistry->GraphAssetIndex[ActiveGraphEditor->GetCurrentGraph()->GraphGuid]->GetNode(
				NodeIndex
			);

			if (GraphNode == nullptr)
			{
				UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- Invalid Node Index --"));
				return;
			}

			switch (EPhraseDirectionalInput(MoveDirection))
			{
				case EPhraseDirectionalInput::UP:
					GraphNode->NodePosY -= MoveAmount;
					break;

				case EPhraseDirectionalInput::DOWN:
					GraphNode->NodePosY += MoveAmount;
					break;

				case EPhraseDirectionalInput::LEFT:
					GraphNode->NodePosX -= MoveAmount;
					break;

				case EPhraseDirectionalInput::RIGHT:
					GraphNode->NodePosX += MoveAmount;
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

	TSharedPtr<FPhraseEventNode> PinConnectEventNode = MakeShared<FPhraseEventNode>();
	PinConnectEventNode->OnPhraseParsed.BindLambda([this](const FParseRecord& Record) {

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

		TSharedPtr<FGraphIndexer> IndexerForGraph = AssetAccessibilityRegistry->GetGraphIndexer(ActiveGraphEditor->GetCurrentGraph());

		// Get Inputs
		TArray<int> NodeInputs; 
		Record.PhraseInputs.MultiFind("NODE_INDEX", NodeInputs, true);

		TArray<int> PinInputs;
		Record.PhraseInputs.MultiFind("PIN_INDEX", PinInputs, true);

		if (NodeInputs.Num() < 2 || PinInputs.Num() < 2)
		{
			UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- Invalid Inputs Length --"));
			return;
		}

		UEdGraphPin* SourcePin = IndexerForGraph->GetPin(
			NodeInputs[0],
			PinInputs[0]
		);

		UEdGraphPin* TargetPin = IndexerForGraph->GetPin(
			NodeInputs[1],
			PinInputs[1]
		);

		if (SourcePin == nullptr || TargetPin == nullptr)
		{
			UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- Invalid Pins --"));
			return;
		}

		SourcePin->MakeLinkTo(TargetPin);
	});

	TSharedPtr<FPhraseEventNode> PinDisconnectEventNode = MakeShared<FPhraseEventNode>();
	PinConnectEventNode->OnPhraseParsed.BindLambda([this](const FParseRecord& Record) {

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

		TSharedPtr<FGraphIndexer> IndexerForGraph = AssetAccessibilityRegistry->GetGraphIndexer(ActiveGraphEditor->GetCurrentGraph());

		// Get Inputs
		TArray<int> NodeInputs;
		Record.PhraseInputs.MultiFind("NODE_INDEX", NodeInputs, true);

		TArray<int> PinInputs;
		Record.PhraseInputs.MultiFind("PIN_INDEX", PinInputs, true);

		if (NodeInputs.Num() < 2 || PinInputs.Num() < 2)
		{
			UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- Invalid Inputs Length --"));
			return;
		}

		UEdGraphPin* SourcePin = IndexerForGraph->GetPin(
			NodeInputs[0],
			PinInputs[0]
		);

		UEdGraphPin* TargetPin = IndexerForGraph->GetPin(
			NodeInputs[1],
			PinInputs[1]
		);

		if (SourcePin == nullptr || TargetPin == nullptr)
		{
			UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- Invalid Pins --"));
			return;
		}

		SourcePin->BreakLinkTo(TargetPin);
	});

	TDelegate<void(int32 IndexInput)> NodeIndexFocusEvent;
	NodeIndexFocusEvent.BindLambda([this](int32 NodeIndex) {
		TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
		if (!ActiveTab.IsValid())
		{
			return;
		}

		SGraphEditor* ActiveGraphEditor = (SGraphEditor*) ActiveTab->GetContent().ToSharedPtr().Get();
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
					}, NodeIndexFocusEvent)
			})
	);

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
	MoveViewportEventNode->OnPhraseParsed.BindLambda([this](const FParseRecord& Record) {
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
		const int MoveDirection = *Record.PhraseInputs.Find("DIRECTION");
		const int MoveAmount = *Record.PhraseInputs.Find("AMOUNT");

		const FString ActiveEditorViewportType = ActiveEditorViewport->GetTypeAsString();
		if (ActiveEditorViewportType == "SGraphEditor")
		{
			SGraphEditor* GraphViewport = (SGraphEditor*) ActiveEditorViewport;

			FVector2D CurrViewLocation;
			float CurrZoomAmount;
			GraphViewport->GetViewLocation(CurrViewLocation, CurrZoomAmount);

			UE_LOG(LogOpenAccessibility, Display, TEXT("MoveViewportEventNode: Curr Zoom Amount: %d"), CurrZoomAmount);
		
			switch (EPhraseDirectionalInput(MoveDirection))
			{
				case EPhraseDirectionalInput::UP:
					CurrViewLocation.Y -= MoveAmount;
					break;

				case EPhraseDirectionalInput::DOWN:
					CurrViewLocation.Y += MoveAmount;
					break;

				case EPhraseDirectionalInput::LEFT:
					CurrViewLocation.X -= MoveAmount;
					break;

				case EPhraseDirectionalInput::RIGHT:
					CurrViewLocation.X += MoveAmount;
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
	ZoomViewportEventNode->OnPhraseParsed.BindLambda([this](const FParseRecord& Record) {
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
		const int ZoomDirection = *Record.PhraseInputs.Find("DIRECTION");
		const int ZoomAmount = *Record.PhraseInputs.Find("AMOUNT");

		const FString ActiveEditorViewportType = ActiveEditorViewport->GetTypeAsString();
		if (ActiveEditorViewportType == "SGraphEditor")
		{
			SGraphEditor* GraphViewport = (SGraphEditor*)ActiveEditorViewport;

			FVector2D CurrViewLocation;
			float CurrZoomAmount;
			GraphViewport->GetViewLocation(CurrViewLocation, CurrZoomAmount);

			UE_LOG(LogOpenAccessibility, Display, TEXT("ZoomViewportEventNode: Curr Zoom Amount: %d"), CurrZoomAmount);

			switch (EPhraseDirectionalInput(ZoomDirection))
			{
				case EPhraseDirectionalInput::UP:
					CurrZoomAmount += ZoomAmount;
					break;

				case EPhraseDirectionalInput::DOWN:
					CurrZoomAmount -= ZoomAmount;
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
	IndexFocusEventNode->OnPhraseParsed.BindLambda([this](const FParseRecord& Record) {
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
		const int IndexToFocus = *Record.PhraseInputs.Find("INDEX");

		const FString ActiveEditorViewportType = ActiveEditorViewport->GetTypeAsString();
		if (ActiveEditorViewportType == "SGraphEditor")
		{
			SGraphEditor* GraphViewport = (SGraphEditor*)ActiveEditorViewport;

			TSharedRef<FGraphIndexer> IndexerForGraph = AssetAccessibilityRegistry->GetGraphIndexer(GraphViewport->GetCurrentGraph());
			UEdGraphNode* GraphNode = IndexerForGraph->GetNode(IndexToFocus);
			
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