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
	MoveEventNode->OnPhraseEvent.BindLambda([this](const FParseRecord& Record) {

		UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Executed --"));

		UE_LOG(LogOpenAccessibility, Display, TEXT(" -- | Vals | Index: %d | Direction: %d | Amount : %d | --"), 
			Record.PhraseInputs["INDEX"], Record.PhraseInputs["DIRECTION"], Record.PhraseInputs["AMOUNT"]
		);

		TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
		if (!ActiveTab.IsValid())
		{
			UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Failed -- No Active Tab --"));
			return;
		}

		SGraphEditor* ActiveGraphEditor = (SGraphEditor*)ActiveTab->GetContent().ToSharedPtr().Get();

		if (ActiveGraphEditor != nullptr)
		{
			UEdGraphNode* GraphNode = AssetAccessibilityRegistry->GraphAssetIndex[ActiveGraphEditor->GetCurrentGraph()->GraphGuid]->GetNode(
				Record.PhraseInputs["INDEX"]
			);

			switch (EPhraseDirectionalInput(Record.PhraseInputs["DIRECTION"]))
			{
				case EPhraseDirectionalInput::UP:
					GraphNode->NodePosY -= Record.PhraseInputs["AMOUNT"];
					break;

				case EPhraseDirectionalInput::DOWN:
					GraphNode->NodePosY += Record.PhraseInputs["AMOUNT"];
					break;

				case EPhraseDirectionalInput::LEFT:
					GraphNode->NodePosX -= Record.PhraseInputs["AMOUNT"];

				case EPhraseDirectionalInput::RIGHT:
					GraphNode->NodePosX += Record.PhraseInputs["AMOUNT"];

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
	PinConnectEventNode->OnPhraseEvent.BindLambda([this](const FParseRecord& Record) {
		
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

		UEdGraphPin* SourcePin = IndexerForGraph->GetPin(
			Record.PhraseInputs["NODE_INDEX"],
			Record.PhraseInputs["PIN_INDEX"]
		);

		UEdGraphPin* TargetPin = IndexerForGraph->GetPin(
			Record.PhraseInputs["NODE_INDEX_2"],
			Record.PhraseInputs["PIN_INDEX_2"]
		);
			
		SourcePin->MakeLinkTo(TargetPin);
	});

	FOpenAccessibilityCommunicationModule::Get().PhraseTree->BindBranch(
		MakeShared<FPhraseNode>(
			TEXT("NODE"),
			TPhraseNodeArray {

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
												
													MakeShared<FPhraseInputNode>(TEXT("NODE_INDEX_2"),
													TPhraseNodeArray {
														
															MakeShared<FPhraseInputNode>(TEXT("PIN_INDEX_2"),
															TPhraseNodeArray {
																PinConnectEventNode
															})
													})
											})
									})
							}),
					})
			})
	);
}

void FOpenAccessibilityModule::ShutdownModule()
{
	UE_LOG(LogOpenAccessibility, Display, TEXT("OpenAccessibilityModule::ShutdownModule()"));

	UnregisterConsoleCommands();
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