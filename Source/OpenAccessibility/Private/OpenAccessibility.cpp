// Copyright Epic Games, Inc. All Rights Reserved.

#include "OpenAccessibility.h"
#include "OpenAccessibilityCommunication.h"
#include "OpenAccessibilityLogging.h"

#include "PhraseTree/PhraseNode.h"
#include "PhraseTree/PhraseInputNode.h"
#include "PhraseTree/PhraseDirectionalInputNode.h"
#include "PhraseTree/PhraseEventNode.h"

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

						MakeShared<FPhraseDirectionalInputNode>(TEXT("DIRECTION"),
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
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOpenAccessibilityModule, OpenAccessibility)