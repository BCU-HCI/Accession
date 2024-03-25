// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GraphIndexer.h"

/**
 * 
 */
class OPENACCESSIBILITY_API FAssetAccessibilityRegistry
{
public:
	FAssetAccessibilityRegistry();
	~FAssetAccessibilityRegistry();

	// Graph Indexing

	bool IsGraphAssetRegistered(const UEdGraph* InGraph) const;
	bool RegisterGraphAsset(const UEdGraph* InGraph);
	bool UnregisterGraphAsset(const UEdGraph* InGraph);

	TSharedRef<FGraphIndexer> GetGraphIndexer(const UEdGraph* InGraph) const {
		return GraphAssetIndex[InGraph->GraphGuid].ToSharedRef();
	}

	void GetAllGraphKeyIndexes(TArray<FGuid>& OutGraphKeys) const;
	TArray<FGuid> GetAllGraphKeyIndexes() const;

	void GetAllGraphIndexes(TArray<TSharedPtr<FGraphIndexer>>& OutGraphIndexes) const;
	TArray<TSharedPtr<FGraphIndexer>> GetAllGraphIndexes();

	// Game World Indexing

	bool IsGameWorldAssetRegistered(const UWorld* InWorld) const;
	bool RegisterGameWorldAsset(const UWorld* InWorld);
	bool UnregisterGameWorldAsset(const UWorld* InWorld);

private:

	// Asset Register Events

	void OnAssetOpenedInEditor(UObject* OpenedAsset, IAssetEditorInstance* EditorInstance);

	void EmptyGraphAssetIndex();
	void EmptyGameWorldAssetIndex();

	// Asset Editor Registers

	void RegisterBlueprintAsset(UBlueprint* InBlueprint);
	void RegisterMaterialAsset(UMaterial* InMaterial);
	void RegisterUWorldAsset(UWorld* InWorld);

public:
	TMap<FGuid, TSharedPtr<FGraphIndexer>> GraphAssetIndex;
	//TMap<UWorld, FWorldIndexer*> GameWorldAssetIndex;

private:

	FDelegateHandle AssetOpenedInEditorHandle;
};
