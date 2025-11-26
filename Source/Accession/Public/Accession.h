// Copyright (C) HCI-BCU 2025. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "AccessionAssetRegistry.h"
#include "OAccessionNodeFactory.h"

class FAccessionModule : public IModuleInterface
{

public:
	/** IModuleInterface Implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	/** End IModuleInterface Implementation */

	static FAccessionModule &Get()
	{
		return FModuleManager::GetModuleChecked<FAccessionModule>("Accession");
	}

	virtual bool SupportsDynamicReloading() override
	{
		return false;
	}

private:
	static void FocusChangeListener(const FFocusEvent &FocusEvent, const FWeakWidgetPath &PrevWidgetPath, const TSharedPtr<SWidget> &PrevFocusedWidget, const FWidgetPath &NewWidgetPath, const TSharedPtr<SWidget> &NewFocusedWidget);

	// Phrase Branch Bindings

	/// <summary>
	/// Binds the Local Locomotion Branch of Commands onto the Phrase Tree.
	/// </summary>
	void BindLocalizedInteractionBranch();

	/// <summary>
	/// Binds the Graph Interaction Branch of Commands onto the Phrase Tree.
	/// </summary>
	void BindGraphInteractionBranch();
	/// <summary>
	/// Binds the Viewport Interaction Branch of Commands onto the Phrase Tree.
	/// </summary>
	void BindViewportInteractionBranch();

	// Transcription Visualization

	/// <summary>
	/// Constructs the Transcription Visualizer.
	/// </summary>
	void CreateTranscriptionVisualization();

	/// <summary>
	/// Destroys the Transcription Visualizer, and Cleans Up any Allocations.
	/// </summary>
	void DestroyTranscriptionVisualization();

	// Console Commands

	/// <summary>
	/// Registers the Console Commands for the Module.
	/// </summary>
	void RegisterConsoleCommands();

	/// <summary>
	/// Unregisters all Previously Registered Console Commands For the Module.
	/// </summary>
	void UnregisterConsoleCommands();

public:
	/// <summary>
	/// The Node Factory for Generating Indexed Graph Nodes.
	/// </summary>
	TSharedPtr<class FAccessionNodeFactory> NodeFactory;

	/// <summary>
	/// The Registry for Any Unreal Asset Information.
	/// </summary>
	TSharedPtr<class FAccessionAssetRegistry> AssetRegistry;

private:
	TObjectPtr<class UTranscriptionVisualizer> TranscriptionVisualizer;

	TArray<IConsoleCommand *> ConsoleCommands;
};
