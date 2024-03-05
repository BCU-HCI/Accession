// Copyright F-Dudley. All Rights Reserved.

#include "AssetAccessibilityRegistry.h"
#include "OpenAccessibilityLogging.h"

#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "Misc/Guid.h"

FAssetAccessibilityRegistry::FAssetAccessibilityRegistry()
{
	GraphAssetIndex = TMap<FGuid, TSharedPtr<FGraphIndexer>>();
	//GameWorldAssetIndex = TMap<FGuid, FGameWorldIndexer*>();

	AssetOpenedInEditorHandle = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OnAssetOpenedInEditor()
		.AddRaw(this, &FAssetAccessibilityRegistry::OnAssetOpenedInEditor);
}

FAssetAccessibilityRegistry::~FAssetAccessibilityRegistry()
{
	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OnAssetOpenedInEditor()
		.Remove(AssetOpenedInEditorHandle);

	EmptyGraphAssetIndex();
}

void FAssetAccessibilityRegistry::OnAssetOpenedInEditor(UObject* OpenedAsset, IAssetEditorInstance* EditorInstance)
{
	UE_LOG(LogOpenAccessibility, Log, TEXT("|| AssetRegistry || Asset { %s } Opened In Editor: { %s } ||"), *OpenedAsset->GetName(), EditorInstance->GetEditorName());

	if (UBlueprint* OpenedBlueprint = Cast<UBlueprint>(OpenedAsset))
	{
		UE_LOG(LogOpenAccessibility, Log, TEXT("|| AssetRegistry || Asset { %s } Is A Blueprint ||"), *OpenedBlueprint->GetName());

		RegisterBlueprintAsset(OpenedBlueprint);
	}
}

bool FAssetAccessibilityRegistry::IsGraphAssetRegistered(const UEdGraph* InUEdGraph) const
{
	return GraphAssetIndex.Find(InUEdGraph->GraphGuid) != nullptr;
}

bool FAssetAccessibilityRegistry::RegisterGraphAsset(const UEdGraph* InGraph)
{
	GraphAssetIndex.Add(InGraph->GraphGuid, MakeShared<FGraphIndexer>(InGraph));

	for (auto& ChildGraph : InGraph->SubGraphs)
	{
		if (!RegisterGraphAsset(ChildGraph))
		{
			UE_LOG(LogOpenAccessibility, Error, TEXT("|| AssetRegistry || Error When Logging Child Graph: { %s } From Parent: { %s }||"), *ChildGraph->GetName(), *InGraph->GetName())
			
			return false;
		}
	}

	return true;
}

bool FAssetAccessibilityRegistry::UnregisterGraphAsset(const UEdGraph* UEdGraph)
{
	GraphAssetIndex.Remove(UEdGraph->GraphGuid);

	for (auto& ChildGraph : UEdGraph->SubGraphs)
	{
		if (!UnregisterGraphAsset(ChildGraph))
		{
			UE_LOG(LogOpenAccessibility, Error, TEXT("|| AssetRegistry || Error When Unregistering Child Graph: { %s } From Parent: { %s }||"), *ChildGraph->GetName(), *UEdGraph->GetName())
			
			return false;
		}
	}

	return true;
}

bool FAssetAccessibilityRegistry::IsGameWorldAssetRegistered(const UWorld* UWorld) const
{
	throw std::exception("The method or operation is not implemented.");
}

bool FAssetAccessibilityRegistry::RegisterGameWorldAsset(const UWorld* UWorld)
{
	throw std::exception("The method or operation is not implemented.");
}

bool FAssetAccessibilityRegistry::UnregisterGameWorldAsset(const UWorld* UWorld)
{
	throw std::exception("The method or operation is not implemented.");
}

void FAssetAccessibilityRegistry::EmptyGraphAssetIndex()
{
	for (auto& GraphIndexer : GraphAssetIndex)
	{
		GraphIndexer.Value.Reset();
	}

	GraphAssetIndex.Empty();
}

void FAssetAccessibilityRegistry::EmptyGameWorldAssetIndex()
{
	throw std::exception("The method or operation is not implemented.");	
}

void FAssetAccessibilityRegistry::RegisterBlueprintAsset(UBlueprint* InBlueprint)
{
	// Register the Blueprint's Graphs
	TArray<UEdGraph*> Graphs;

	InBlueprint->GetAllGraphs(Graphs);
	for (auto& Graph : Graphs)
	{
		RegisterGraphAsset(Graph);
	}

	// Register the Blueprint's World
	// Some Blueprints have no connected World / GameObjects, 
	// so we need to check if the World is valid

	UWorld* BlueprintDebugWorld = InBlueprint->GetWorldBeingDebugged();
	if (BlueprintDebugWorld != nullptr)
	{
		RegisterUWorldAsset(BlueprintDebugWorld);
	}
}

void FAssetAccessibilityRegistry::RegisterUWorldAsset(UWorld* InWorld)
{
	throw std::exception("The method or operation is not implemented.");
} 
