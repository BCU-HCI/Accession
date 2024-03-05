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

	// Asset Register Events

	void OnAssetOpenedInEditor(UObject* OpenedAsset, IAssetEditorInstance* EditorInstance);

	// Graph Indexing

	bool IsGraphAssetRegistered(const UEdGraph* InGraph) const;
	bool RegisterGraphAsset(const UEdGraph* InGraph);
	bool UnregisterGraphAsset(const UEdGraph* InGraph);

	// Game World Indexing

	bool IsGameWorldAssetRegistered(const UWorld* InWorld) const;
	bool RegisterGameWorldAsset(const UWorld* InWorld);
	bool UnregisterGameWorldAsset(const UWorld* InWorld);

private:

	void EmptyGraphAssetIndex();
	void EmptyGameWorldAssetIndex();

	// Asset Editor Registers

	void RegisterBlueprintAsset(UBlueprint* InBlueprint);
	void RegisterUWorldAsset(UWorld* InWorld);

private:
	TMap<FGuid, TSharedPtr<FGraphIndexer>> GraphAssetIndex;
	//TMap<UWorld, FWorldIndexer*> GameWorldAssetIndex;

	FDelegateHandle AssetOpenedInEditorHandle;
};
