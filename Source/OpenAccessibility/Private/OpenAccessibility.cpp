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
	TSharedPtr<FPhraseEventNode> EventNode = MakeShared<FPhraseEventNode>();
	EventNode->OnPhraseEvent.BindLambda([this](const FParseRecord& Record) {
		UE_LOG(LogOpenAccessibility, Display, TEXT(" -- DEMO PHRASE_TREE Event Executed --"));
		UE_LOG(LogOpenAccessibility, Display, TEXT(" -- | Vals | Index: %d | Direction: %d | Amount : %d | --"), 
			Record.PhraseInputs["INDEX"], Record.PhraseInputs["DIRECTION"], Record.PhraseInputs["AMOUNT"]
		);

		for (auto& Index : AssetAccessibilityRegistry->GetAllGraphKeyIndexes())
		{
			AssetAccessibilityRegistry->GraphAssetIndex[Index]->GetNode(
				Record.PhraseInputs["INDEX"]
			)->NodePosY -= Record.PhraseInputs["AMOUNT"];
		}
	});

	FOpenAccessibilityCommunicationModule::Get().PhraseTree->BindBranch(
		MakeShared<FPhraseNode>(
			TEXT("NODE"),
			TPhraseNodeArray {

				MakeShared<FPhraseInputNode>(TEXT("INDEX"),
				TPhraseNodeArray {

					MakeShared<FPhraseNode>(TEXT("MOVE"),
					TPhraseNodeArray {

						MakeShared<FPhrase2DDirectionalInputNode>(TEXT("DIRECTION"),
						TPhraseNodeArray {

								MakeShared<FPhraseInputNode>(TEXT("AMOUNT"),
								TPhraseNodeArray {
									EventNode
								})
						})
					})
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
	ConsoleVariables.Add(IConsoleManager::Get().RegisterConsoleCommand(
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

}

void FOpenAccessibilityModule::UnregisterConsoleCommands()
{
	IConsoleCommand* ConsoleVariable = nullptr;
	while (ConsoleVariables.Num() > 0)
	{
		ConsoleVariable = ConsoleVariables.Pop();

		IConsoleManager::Get().UnregisterConsoleObject(ConsoleVariable);

		delete ConsoleVariable;
		ConsoleVariable = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOpenAccessibilityModule, OpenAccessibility)