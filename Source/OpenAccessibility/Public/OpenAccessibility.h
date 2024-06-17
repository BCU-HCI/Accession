// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "AssetAccessibilityRegistry.h"
#include "OAccessibilityNodeFactory.h"
#include "AccessibilityWrappers/AccessibilityAddNodeContextMenu.h"

class FOpenAccessibilityModule : public IModuleInterface
{

public:

	/** IModuleInterface Implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	/** End IModuleInterface Implementation */

	static FOpenAccessibilityModule& Get()
	{
		return FModuleManager::GetModuleChecked<FOpenAccessibilityModule>("OpenAccessibility");
	}

	virtual bool SupportsDynamicReloading() override
	{
		return false;
	}

private:

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

	// Accessibility Components

	/// <summary>
	/// The Node Factory for Generating Accessibility Graph Nodes.
	/// </summary>
	TSharedPtr<class FAccessibilityNodeFactory> AccessibilityNodeFactory;

	/// <summary>
	/// The Registry for Any Asset Accessibility Information.
	/// </summary>
	TSharedPtr<class FAssetAccessibilityRegistry> AssetAccessibilityRegistry;

private:

	TSharedPtr<class FTranscriptionVisualizer> TranscriptionVisualizer;

	TArray<IConsoleCommand*> ConsoleCommands;
};
