// Copyright (C) HCI-BCU 2025. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"
#include "Delegates/DelegateCombinations.h"

#include "PhraseTree.h"
#include "PhraseTreeUtils.h"

// UDELEGATE()
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTranscriptionRecievedSignature, const TArray<FString>, InTranscription);

class FAccessionCommunicationModule : public IModuleInterface
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

	static FAccessionCommunicationModule &Get()
	{
		return FModuleManager::GetModuleChecked<FAccessionCommunicationModule>("AccessionCommunication");
	}

private:
	/// <summary>
	/// Registers the Console Commands for This Module.
	/// </summary>
	void RegisterConsoleCommands();

	/// <summary>
	/// Unregisters any Console Commands Registered by This Module.
	/// </summary>
	void UnregisterConsoleCommands();

private:

	TArray<IConsoleCommand *> ConsoleCommands;
};
