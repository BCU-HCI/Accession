// Copyright (C) HCI-BCU. All rights reserved.
// Published under GPLv3 License, 2025. See LICENSE in the Plugin Root for more information.

#pragma once

#include "CoreMinimal.h"

#include "GraphIndexer.h"

class UBehaviorTree;

/**
 *
 */
class ACCESSION_API FAccessionAssetRegistry
{
public:
	FAccessionAssetRegistry();
	~FAccessionAssetRegistry();

	// Graph Indexing

	/// <summary>
	/// Checks if the provied graph asset has been registered with the registry.
	/// </summary>
	/// <param name="InGraph">The Graph Asset to Check.</param>
	/// <returns>True, if the graph has been registered. False, if the graph has not.</returns>
	bool IsGraphAssetRegistered(const UEdGraph *InGraph) const;

	/// <summary>
	/// Registers the provided graph asset with the registry.
	/// </summary>
	/// <param name="InGraph">The Graph to Register.</param>
	/// <returns>True, if the Graph was Successfully Registered. False, if the Graph Could Not Be Registered.</returns>
	bool RegisterGraphAsset(const UEdGraph *InGraph);

	bool RegisterGraphAsset(const UEdGraph *InGraph, const TSharedRef<FGraphIndexer> InGraphIndexer);

	/// <summary>
	/// Unregisters the provided graph asset from the registry.
	/// </summary>
	/// <param name="InGraph">The Graph To Unregister.</param>
	/// <returns>True, if the provided graph was unregistered successfully. False, if the Graph Could Not Be Unregistered.</returns>
	bool UnregisterGraphAsset(const UEdGraph *InGraph);

	/// <summary>
	/// Gets the Graph Indexer linked to the provided graph asset.
	/// </summary>
	/// <param name="InGraph">The Graph to Find the Linked Indexer For.</param>
	/// <returns>Returns the Found SharedReference of the GraphIndexer when found successfully. Returns nullptr on fail.</returns>
	TSharedRef<FGraphIndexer> GetGraphIndexer(const UEdGraph *InGraph) const
	{
		if (GraphAssetIndex.Contains(InGraph->GraphGuid))
			return GraphAssetIndex[InGraph->GraphGuid].ToSharedRef();

		return TSharedRef<FGraphIndexer>();
	}

	/// <summary>
	/// Gets the Guids of all the Graphs that have been registered with the registry.
	/// </summary>
	/// <param name="OutGraphKeys">The Array of Guids to Apply the found Guids to.</param>
	void GetAllGraphKeyIndexes(TArray<FGuid> &OutGraphKeys) const;

	/// <summary>
	/// Gets the Guids of all the Graphs that have been registered with the registry.
	/// </summary>
	/// <returns>An Array of all Found Guids registered with the registry.</returns>
	TArray<FGuid> GetAllGraphKeyIndexes() const;

	/// <summary>
	/// Gets all the Graph Indexers that have been registered with the registry.
	/// </summary>
	/// <param name="OutGraphIndexes">The Array to apply all the registered graph indexers.</param>
	void GetAllGraphIndexes(TArray<TSharedPtr<FGraphIndexer>> &OutGraphIndexes) const;

	/// <summary>
	/// Gets all the Graph Indexers that have been registered with the registry.
	/// </summary>
	/// <returns>An Array of all the Found Graph Indexers registered with the registry.</returns>
	TArray<TSharedPtr<FGraphIndexer>> GetAllGraphIndexes();

	// Game World Indexing

	/// <summary>
	/// Checks if the provided world asset has been registered with the registry.
	/// </summary>
	/// <param name="InWorld">The UWorld Asset to Check if Registered</param>
	/// <returns>True, if the UWorld Asset is Registered. False, if the UWorld Asset is not.</returns>
	bool IsGameWorldAssetRegistered(const UWorld *InWorld) const;

	/// <summary>
	/// Registered the UWorld Asset with the Registry.
	/// </summary>
	/// <param name="InWorld">The UWorld Asset to Register.</param>
	/// <returns>True, if the Asset was Successfully Registered. False, if the asset could not be registered.</returns>
	bool RegisterGameWorldAsset(const UWorld *InWorld);

	/// <summary>
	/// Unregisters the provided UWorld Asset from the Registry.
	/// </summary>
	/// <param name="InWorld">The UWorld Asset to Unregister.</param>
	/// <returns>True, if the Asset was Successfully Registered. False, if the asset could not be registered.</returns>
	bool UnregisterGameWorldAsset(const UWorld *InWorld);

private:
	// Asset Register Events

	/// <summary>
	/// Callback Function for when an asset is opened in the editor.
	/// </summary>
	/// <param name="OpenedAsset">The Base Object of the Opened Asset. (Can be Downcast)</param>
	/// <param name="EditorInstance">The Instance of the Editor that has been opened for the asset.</param>
	void OnAssetOpenedInEditor(UObject *OpenedAsset, IAssetEditorInstance *EditorInstance);

	/// <summary>
	/// Callback Function for when an assets editor is closed in the editor.
	/// </summary>
	/// <param name="ClosingAsset">The Base Object of the Opened Asset. (Can be Downcast)</param>
	/// <param name="CloseReason">The Reason for the Editor being closed.</param>
	void OnAssetEditorRequestClose(UObject *ClosingAsset, EAssetEditorCloseReason CloseReason);

	/// <summary>
	/// Empties the Graph Asset Index.
	/// </summary>
	void EmptyGraphAssetIndex();

	/// <summary>
	/// Empties the Game World Asset Index.
	/// </summary>
	void EmptyGameWorldAssetIndex();

	// Asset Editor Registers

	/// <summary>
	/// Registers the provided Blueprint Asset, and its componets, with the Registry.
	/// </summary>
	/// <param name="InBlueprint">The UBlueprint Asset To Register.</param>
	void RegisterBlueprintAsset(const UBlueprint *InBlueprint);

	/// <summary>
	/// Registers the provided Material Asset with the Registry.
	/// </summary>
	/// <param name="InMaterial">The UMaterial Assset to Register.</param>
	void RegisterMaterialAsset(const UMaterial *InMaterial);

	/// <summary>
	/// Registers the provided Behavior Tree Asset, and its core components, with the Registry.
	/// </summary>
	void RegisterBehaviorTreeAsset(const UBehaviorTree *InBehaviorTree);

	/// <summary>
	/// Registers the provided UWorld Asset with the Registry.
	/// </summary>
	/// <param name="InWorld">The UWorld Asset to Register.</param>
	void RegisterUWorldAsset(const UWorld *InWorld);

public:
	/// <summary>
	/// A Map containing all the Graph Indexers that have been created for the registered Graph Assets.
	/// </summary>
	TMap<FGuid, TSharedPtr<FGraphIndexer>> GraphAssetIndex;

	/// <summary>
	/// A Map containing all the Game World Indexers that have been created for the registered Game World Assets.
	/// </summary>
	// TMap<UWorld, FWorldIndexer*> GameWorldAssetIndex;

private:
	FDelegateHandle AssetOpenedInEditorHandle;
	FDelegateHandle AssetEditorRequestCloseHandle;
};
