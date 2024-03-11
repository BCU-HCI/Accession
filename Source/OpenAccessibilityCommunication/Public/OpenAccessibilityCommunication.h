// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"
#include "Delegates/DelegateCombinations.h"

//UDELEGATE()
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTranscriptionRecievedSignature, const TArray<FString>, InTranscription);

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
		return FModuleManager::GetModuleChecked<FOpenAccessibilityCommunicationModule>("OpenAccessibilityCommunication");
	}

	bool Tick(const float DeltaTime);

	void HandleKeyDownEvent(const FKeyEvent& InKeyEvent);

	void TranscribeWaveForm(TArray<float> AudioBufferToTranscribe);

private: 

	void LoadZMQDLL();

	void UnloadZMQDLL();
public:

	class UAudioManager* AudioManager;
	TSharedPtr<class FSocketCommunicationServer> SocketServer;

	TUniquePtr<class FPhraseTree> PhraseTree;

private:
	FTickerDelegate TickDelegate;
	FTSTicker::FDelegateHandle TickDelegateHandle;

	FDelegateHandle KeyDownEventHandle;

	void* ZMQDllHandle;
};
