// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"


class FOpenAccessibilityAnalyticsModule : public IModuleInterface {

public:

	/** IModuleInterface Implementation */

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual bool SupportsDynamicReloading() override { return false; }

	/** End IModuleInterface Implementation */

	static FOpenAccessibilityAnalyticsModule& Get() 
	{
		return FModuleManager::GetModuleChecked<FOpenAccessibilityAnalyticsModule>("OpenAccessibilityAnalytics");
	}
};
