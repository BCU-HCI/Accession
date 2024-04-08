// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "AssetAccessibilityRegistry.h"
#include "OAccessibilityNodeFactory.h"

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
	void BindLocalLocomotionBranch();
	void BindGraphInteractionBranch();
	void BindViewportInteractionBranch();

	// Console Commands
	void RegisterConsoleCommands();

	void UnregisterConsoleCommands();

public:

	// Accessibility Components

	TSharedPtr<class FAccessibilityNodeFactory> AccessibilityNodeFactory;
	TSharedPtr<class FAssetAccessibilityRegistry> AssetAccessibilityRegistry;

private:

	TArray<IConsoleCommand*> ConsoleCommands;
};
