// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class UAudioManager;

class FOpenAccessibilityCommunicationModule : public IModuleInterface
{

public:

	/** IModuleInterface Implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual bool SupportsDynamicReloading() override
	{
		return false;
	}
	/** End IModuleInterface Implementation */

	static FOpenAccessibilityCommunicationModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FOpenAccessibilityCommunicationModule>("OpenAccessibilityCommunication");
	}

	void HandleKeyDownEvent(const FKeyEvent& InKeyEvent);

public:
	UAudioManager* AudioManager;

private:
	FDelegateHandle KeyDownEventHandle;
};
