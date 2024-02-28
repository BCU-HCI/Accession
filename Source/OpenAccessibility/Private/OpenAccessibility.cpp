// Copyright Epic Games, Inc. All Rights Reserved.

#include "OpenAccessibility.h"
#include "OpenAccessibilityLogging.h"

#include "SGraphNode.h"
#include "EdGraphUtilities.h"
#include "Containers/Ticker.h"
#include "Logging/StructuredLog.h"
#include "InputCoreTypes.h"


#define LOCTEXT_NAMESPACE "FOpenAccessibilityModule"

void FOpenAccessibilityModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogOpenAccessibility, Display, TEXT("OpenAccessibilityModule::StartupModule()"));

	// Register Asset Editor Subsystem Lambdas || TEMPORARY ||
	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();

	AssetEditorSubsystem->OnAssetOpenedInEditor()
		.AddLambda([](UObject* Asset, IAssetEditorInstance* EditorInstance) {

			UE_LOGFMT(LogOpenAccessibility, Log, "|| Asset Opened || Editor Used: {0} | Asset Name: {1} ||", EditorInstance->GetEditorName(), Asset->GetName());

			// Then Parse the Asset based on Editor Type, and forward to Accessibility Asset Manager 
			// that will manage the Accessibility side of the Assets Editing lifecycle.

			if (EditorInstance->GetEditorName() == "BlueprintEditor")
			{
				if (UBlueprint* Blueprint = Cast<UBlueprint>(Asset))
				{
					TArray<UEdGraph*> Graphs;
					Blueprint->GetAllGraphs(Graphs);

					UE_LOGFMT(LogOpenAccessibility, Log, "|| Asset Info || Graph Amount: {0} ||", Graphs.Num());
				
					// check(GraphEditor.IsValid());

					for (UEdGraph* Graph : Graphs)
					{
						UE_LOGFMT(LogOpenAccessibility, Log, "|| Asset Info || Graph Name: {0} ||", Graph->GetName());

						TSharedPtr<SGraphEditor> GraphEditor = SGraphEditor::FindGraphEditorForGraph(Graph);

						UE_LOGFMT(LogOpenAccessibility, Log, "|| Asset Info || Graph Editor: {0} ||", GraphEditor.IsValid() ? "Valid" : "Invalid");

						for (UEdGraphNode* Node : Graph->Nodes)
						{
							UE_LOGFMT(LogOpenAccessibility, Log, "|| Asset Info || Node Title: {0} ||", Node->GetName());

							// Create Accessibility Node
							// TSharedPtr<SGraphNode> AccessibilityNode = FAccessibilityNodeFactory::CreateNode(Node);

							// Wrap the Node with Accessibility Node
							// Node->WrapWithAccessibilityNode(AccessibilityNode);

							//FString NodeName = Node->GetName();
							//NodeName.AppendChars("_[0]", 4);

							//Node->Rename(NodeName.GetCharArray().GetData(), Node->GetOuter());
						}
					}
				}
			}
		});

	// Register Tick Function
	// TickDelegate = FTickerDelegate::CreateRaw(this, &FOpenAccessibilityModule::Tick);
	// TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(TickDelegate);

	// Register the Accessibility Node Factory
	AccessibilityNodeFactory = MakeShared<FAccessibilityNodeFactory, ESPMode::ThreadSafe>();
	FEdGraphUtilities::RegisterVisualNodeFactory(AccessibilityNodeFactory);
}

bool FOpenAccessibilityModule::Tick(float DeltaTime) // Not Enabled
{
	// This function may be called during the module's execution to perform ticking.
	// For modules that support dynamic reloading, we call this function during the module's execution.

	return true;
}

void FOpenAccessibilityModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UE_LOG(LogOpenAccessibility, Display, TEXT("OpenAccessibilityModule::ShutdownModule()"));

	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOpenAccessibilityModule, OpenAccessibility)