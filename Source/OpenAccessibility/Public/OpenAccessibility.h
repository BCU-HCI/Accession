// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "OAccessibilityNodeFactory.h"

class FOpenAccessibilityModule : public IModuleInterface
{

public:

	/** IModuleInterface Implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	/** End IModuleInterface Implementation */

	bool Tick(float DeltaTime);

	static FOpenAccessibilityModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FOpenAccessibilityModule>("OpenAccessibility");
	}

	virtual bool SupportsDynamicReloading() override
	{
		return false;
	}

public:
	TSharedPtr<class FAccessibilityNodeFactory> AccessibilityNodeFactory;

private:
	FTickerDelegate TickDelegate;
	FTSTicker::FDelegateHandle TickDelegateHandle;

};
