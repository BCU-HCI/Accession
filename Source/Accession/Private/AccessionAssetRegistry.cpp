// Copyright (C) HCI-BCU 2025. All rights reserved.

#include "AccessionAssetRegistry.h"
#include "AccessionLogging.h"
#include "BehaviorTree/BehaviorTree.h"

#include "Subsystems/AssetEditorSubsystem.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "MaterialGraph/MaterialGraph.h"

#include "UObject/Class.h"
#include "Misc/Guid.h"

FAccessionAssetRegistry::FAccessionAssetRegistry()
{
	GraphAssetIndex = TMap<FGuid, TSharedPtr<FGraphIndexer>>();
	// GameWorldAssetIndex = TMap<FGuid, FGameWorldIndexer*>();

	AssetOpenedInEditorHandle = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OnAssetOpenedInEditor().AddRaw(this, &FAccessionAssetRegistry::OnAssetOpenedInEditor);

	AssetEditorRequestCloseHandle = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OnAssetEditorRequestClose().AddRaw(this, &FAccessionAssetRegistry::OnAssetEditorRequestClose);
}

FAccessionAssetRegistry::~FAccessionAssetRegistry()
{
	EmptyGraphAssetIndex();


	if (GEditor == nullptr)
		return;

	UAssetEditorSubsystem* AESubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	if (AESubsystem == nullptr)
		return;

	AESubsystem->OnAssetOpenedInEditor().Remove(AssetOpenedInEditorHandle);
	AESubsystem->OnAssetEditorRequestClose().Remove(AssetEditorRequestCloseHandle);
}

void FAccessionAssetRegistry::OnAssetOpenedInEditor(UObject *OpenedAsset, IAssetEditorInstance *EditorInstance)
{
	UE_LOG(LogAccession, Log, TEXT("|| AssetRegistry || Asset { %s } Opened In Editor: { %s } ||"), *OpenedAsset->GetName(), *EditorInstance->GetEditorName().ToString());

	// Find Asset Type for correct Parsing.
	if (UBlueprint *OpenedBlueprint = Cast<UBlueprint>(OpenedAsset))
	{
		UE_LOG(LogAccession, Log, TEXT("|| AssetRegistry || Asset { %s } Is A Blueprint ||"), *OpenedBlueprint->GetName());

		RegisterBlueprintAsset(OpenedBlueprint);
	}
	else if (UMaterial *OpenedMaterial = Cast<UMaterial>(OpenedAsset))
	{
		UE_LOG(LogAccession, Log, TEXT("|| AssetRegistry || Asset { %s } Is A Material ||"), *OpenedMaterial->GetName());

		RegisterMaterialAsset(OpenedMaterial);
	}
	else if (UBehaviorTree *OpenedBehaviorTree = Cast<UBehaviorTree>(OpenedAsset))
	{
		UE_LOG(LogAccession, Log, TEXT("|| AssetRegistry || Asset { %s } Is A Behavior Tree ||"), *OpenedBehaviorTree->GetName());

		RegisterBehaviorTreeAsset(OpenedBehaviorTree);
	}
}

void FAccessionAssetRegistry::OnAssetEditorRequestClose(UObject *ClosingAsset, EAssetEditorCloseReason CloseReason)
{
	if (ClosingAsset == nullptr)
		return;

	UE_LOG(LogAccession, Log, TEXT("|| AssetRegistry || Asset { %s } Closed | Reason: { %d } ||"), *ClosingAsset->GetFName().ToString(), int64(CloseReason));
}

bool FAccessionAssetRegistry::IsGraphAssetRegistered(const UEdGraph *InUEdGraph) const
{
	return GraphAssetIndex.Contains(InUEdGraph->GraphGuid);
}

bool FAccessionAssetRegistry::RegisterGraphAsset(const UEdGraph *InGraph)
{
	if (!InGraph->IsValidLowLevel())
		return false;

	GraphAssetIndex.Add(InGraph->GraphGuid, MakeShared<FGraphIndexer>(InGraph));

	for (auto &ChildGraph : InGraph->SubGraphs)
	{
		if (!RegisterGraphAsset(ChildGraph))
		{
			UE_LOG(LogAccession, Error, TEXT("|| AssetRegistry || Error When Logging Child Graph: { %s } From Parent: { %s }||"), *ChildGraph->GetName(), *InGraph->GetName())

			return false;
		}
	}

	return true;
}

bool FAccessionAssetRegistry::RegisterGraphAsset(const UEdGraph *InGraph, const TSharedRef<FGraphIndexer> InGraphIndexer)
{
	if (!InGraph->IsValidLowLevel())
		return false;

	GraphAssetIndex.Add(InGraph->GraphGuid, InGraphIndexer.ToSharedPtr());

	for (auto &ChildGraph : InGraph->SubGraphs)
	{
		if (!RegisterGraphAsset(ChildGraph))
		{
			UE_LOG(LogAccession, Error, TEXT("|| AssetRegistry || Error When Logging Child Graph: { %s } From Parent: { %s} ||"), *ChildGraph->GetName(), *InGraph->GetName());
			return false;
		}
	}

	return true;
}

bool FAccessionAssetRegistry::UnregisterGraphAsset(const UEdGraph *UEdGraph)
{
	GraphAssetIndex.Remove(UEdGraph->GraphGuid);

	for (auto &ChildGraph : UEdGraph->SubGraphs)
	{
		if (!UnregisterGraphAsset(ChildGraph))
		{
			UE_LOG(LogAccession, Error, TEXT("|| AssetRegistry || Error When Unregistering Child Graph: { %s } From Parent: { %s }||"), *ChildGraph->GetName(), *UEdGraph->GetName())

			return false;
		}
	}

	return true;
}

void FAccessionAssetRegistry::GetAllGraphKeyIndexes(TArray<FGuid> &OutGraphKeys) const
{
	GraphAssetIndex.GetKeys(OutGraphKeys);
}

TArray<FGuid> FAccessionAssetRegistry::GetAllGraphKeyIndexes() const
{
	TArray<FGuid> GraphKeys;
	GraphAssetIndex.GetKeys(GraphKeys);

	return GraphKeys;
}

void FAccessionAssetRegistry::GetAllGraphIndexes(TArray<TSharedPtr<FGraphIndexer>> &OutGraphIndexes) const
{
	return GraphAssetIndex.GenerateValueArray(OutGraphIndexes);
}

TArray<TSharedPtr<FGraphIndexer>> FAccessionAssetRegistry::GetAllGraphIndexes()
{
	TArray<TSharedPtr<FGraphIndexer>> GraphIndexArray;

	GraphAssetIndex.GenerateValueArray(GraphIndexArray);

	return GraphIndexArray;
}

bool FAccessionAssetRegistry::IsGameWorldAssetRegistered(const UWorld *UWorld) const
{
	throw std::exception("The method or operation is not implemented.");
}

bool FAccessionAssetRegistry::RegisterGameWorldAsset(const UWorld *UWorld)
{
	throw std::exception("The method or operation is not implemented.");
}

bool FAccessionAssetRegistry::UnregisterGameWorldAsset(const UWorld *UWorld)
{
	throw std::exception("The method or operation is not implemented.");
}

void FAccessionAssetRegistry::EmptyGraphAssetIndex()
{
	for (auto &GraphIndexer : GraphAssetIndex)
	{
		GraphIndexer.Value.Reset();
	}

	GraphAssetIndex.Empty();
}

void FAccessionAssetRegistry::EmptyGameWorldAssetIndex()
{
	throw std::exception("The method or operation is not implemented.");
}

void FAccessionAssetRegistry::RegisterBlueprintAsset(const UBlueprint *InBlueprint)
{
	// Register the Blueprint's Graphs
	TArray<UEdGraph *> Graphs;

	InBlueprint->GetAllGraphs(Graphs);
	for (auto &Graph : Graphs)
	{
		RegisterGraphAsset(Graph);
	}

	// Register the Blueprint's World
	// Some Blueprints have no connected World / GameObjects,
	// so we need to check if the World is valid

	UWorld *BlueprintDebugWorld = InBlueprint->GetWorldBeingDebugged();
	if (BlueprintDebugWorld != nullptr)
	{
		RegisterUWorldAsset(BlueprintDebugWorld);
	}
}

void FAccessionAssetRegistry::RegisterMaterialAsset(const UMaterial *InMaterial)
{
	if (!InMaterial->MaterialGraph)
		return;

	TSharedPtr<FGraphIndexer> GraphIndexer = MakeShared<FGraphIndexer>(InMaterial->MaterialGraph.Get());

	RegisterGraphAsset(InMaterial->MaterialGraph.Get(), GraphIndexer.ToSharedRef());
}

void FAccessionAssetRegistry::RegisterBehaviorTreeAsset(const UBehaviorTree *InBehaviorTree)
{
	if (InBehaviorTree->BTGraph->IsValidLowLevel())
	{
		RegisterGraphAsset(InBehaviorTree->BTGraph);
	}
}

void FAccessionAssetRegistry::RegisterUWorldAsset(const UWorld *InWorld)
{
	throw std::exception("The method or operation is not implemented.");
}
