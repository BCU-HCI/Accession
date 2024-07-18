// Copyright F-Dudley. All Rights Reserved.

#include "AssetAccessibilityRegistry.h"
#include "OpenAccessibilityLogging.h"
#include "BehaviorTree/BehaviorTree.h"

#include "Subsystems/AssetEditorSubsystem.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "MaterialGraph/MaterialGraph.h"

#include "UObject/Class.h"
#include "Misc/Guid.h"

FAssetAccessibilityRegistry::FAssetAccessibilityRegistry()
{
	GraphAssetIndex = TMap<FGuid, TSharedPtr<FGraphIndexer>>();
	//GameWorldAssetIndex = TMap<FGuid, FGameWorldIndexer*>();

	AssetOpenedInEditorHandle = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OnAssetOpenedInEditor()
		.AddRaw(this, &FAssetAccessibilityRegistry::OnAssetOpenedInEditor);

	AssetEditorRequestCloseHandle = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OnAssetEditorRequestClose()
		.AddRaw(this, &FAssetAccessibilityRegistry::OnAssetEditorRequestClose);
}

FAssetAccessibilityRegistry::~FAssetAccessibilityRegistry()
{
	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OnAssetOpenedInEditor()
		.Remove(AssetOpenedInEditorHandle);

	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OnAssetEditorRequestClose()
		.Remove(AssetEditorRequestCloseHandle);

	EmptyGraphAssetIndex();
}

void FAssetAccessibilityRegistry::OnAssetOpenedInEditor(UObject* OpenedAsset, IAssetEditorInstance* EditorInstance)
{
	UE_LOG(LogOpenAccessibility, Log, TEXT("|| AssetRegistry || Asset { %s } Opened In Editor: { %s } ||"), *OpenedAsset->GetName(), *EditorInstance->GetEditorName().ToString());

	if (UBlueprint* OpenedBlueprint = Cast<UBlueprint>(OpenedAsset))
	{
		UE_LOG(LogOpenAccessibility, Log, TEXT("|| AssetRegistry || Asset { %s } Is A Blueprint ||"), *OpenedBlueprint->GetName());

		RegisterBlueprintAsset(OpenedBlueprint);
	}
	else if (UMaterial* OpenedMaterial = Cast<UMaterial>(OpenedAsset))
	{
		UE_LOG(LogOpenAccessibility, Log, TEXT("|| AssetRegistry || Asset { %s } Is A Material ||"), *OpenedMaterial->GetName());
	
		RegisterMaterialAsset(OpenedMaterial);
	}
	else if (UBehaviorTree* OpenedBehaviorTree = Cast<UBehaviorTree>(OpenedAsset))
	{
		UE_LOG(LogOpenAccessibility, Log, TEXT("|| AssetRegistry || Asset { %s } Is A Behavior Tree ||"), *OpenedBehaviorTree->GetName());

		RegisterBehaviorTreeAsset(OpenedBehaviorTree);
	}
}

void FAssetAccessibilityRegistry::OnAssetEditorRequestClose(UObject* ClosingAsset, EAssetEditorCloseReason CloseReason)
{
	if (ClosingAsset == nullptr)
		return;

	UE_LOG(LogOpenAccessibility, Log, TEXT("|| AssetRegistry || Asset { %s } Closed | Reason: { %d } ||"), *ClosingAsset->GetFName().ToString(), int64(CloseReason));
}

bool FAssetAccessibilityRegistry::IsGraphAssetRegistered(const UEdGraph* InUEdGraph) const
{
	return GraphAssetIndex.Contains(InUEdGraph->GraphGuid);
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

bool FAssetAccessibilityRegistry::RegisterGraphAsset(const UEdGraph* InGraph, const TSharedRef<FGraphIndexer> InGraphIndexer)
{
	GraphAssetIndex.Add(InGraph->GraphGuid, InGraphIndexer.ToSharedPtr());

	for (auto& ChildGraph : InGraph->SubGraphs)
	{
		if (!RegisterGraphAsset(ChildGraph))
		{
			UE_LOG(LogOpenAccessibility, Error, TEXT("|| AssetRegistry || Error When Logging Child Graph: { %s } From Parent: { %s} ||"), *ChildGraph->GetName(), *InGraph->GetName());
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

void FAssetAccessibilityRegistry::GetAllGraphKeyIndexes(TArray<FGuid>& OutGraphKeys) const
{
	GraphAssetIndex.GetKeys(OutGraphKeys);
}

TArray<FGuid> FAssetAccessibilityRegistry::GetAllGraphKeyIndexes() const
{
	TArray<FGuid> GraphKeys;
	GraphAssetIndex.GetKeys(GraphKeys);

	return GraphKeys;
}

void FAssetAccessibilityRegistry::GetAllGraphIndexes(TArray<TSharedPtr<FGraphIndexer>>& OutGraphIndexes) const
{
	return GraphAssetIndex.GenerateValueArray(OutGraphIndexes);
}

TArray<TSharedPtr<FGraphIndexer>> FAssetAccessibilityRegistry::GetAllGraphIndexes()
{
	TArray<TSharedPtr<FGraphIndexer>> GraphIndexArray;
	
	GraphAssetIndex.GenerateValueArray(GraphIndexArray);

	return GraphIndexArray;
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

void FAssetAccessibilityRegistry::RegisterBlueprintAsset(const UBlueprint* InBlueprint)
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

void FAssetAccessibilityRegistry::RegisterMaterialAsset(const UMaterial* InMaterial)
{
	if (InMaterial->MaterialGraph->IsValidLowLevel())
	{
		TSharedPtr<FGraphIndexer> GraphIndexer = MakeShared<FGraphIndexer>(InMaterial->MaterialGraph.Get());

		RegisterGraphAsset(InMaterial->MaterialGraph.Get(), GraphIndexer.ToSharedRef());
	}
}

void FAssetAccessibilityRegistry::RegisterBehaviorTreeAsset(const UBehaviorTree* InBehaviorTree)
{
	if (InBehaviorTree->BTGraph->IsValidLowLevel())
	{
		RegisterGraphAsset(InBehaviorTree->BTGraph);
	}
}

void FAssetAccessibilityRegistry::RegisterUWorldAsset(const UWorld* InWorld)
{
	throw std::exception("The method or operation is not implemented.");
} 
