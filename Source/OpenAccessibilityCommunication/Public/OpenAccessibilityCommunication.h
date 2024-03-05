// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

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

	static FOpenAccessibilityCommunicationModule* Get()
	{
		static const FName ModuleName = FName("OpenAccessibilityCommunication");
		return FModuleManager::GetModulePtr<FOpenAccessibilityCommunicationModule>(ModuleName);
	}

	bool Tick(const float DeltaTime);

	void HandleKeyDownEvent(const FKeyEvent& InKeyEvent);

	void OnTranscriptionReady(TArray<float> AudioBufferToTranscribe);
private:

	void LoadZMQDLL();

	void UnloadZMQDLL();
public:
	class UAudioManager* AudioManager;
	TSharedPtr<class FSocketCommunicationServer> SocketServer;

private:
	FTickerDelegate TickDelegate;
	FTSTicker::FDelegateHandle TickDelegateHandle;

	FDelegateHandle KeyDownEventHandle;

	void* ZMQDllHandle;
};
