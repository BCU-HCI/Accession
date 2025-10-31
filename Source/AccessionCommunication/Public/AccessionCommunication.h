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

	bool Tick(const float DeltaTime);

	void HandleKeyDownEvent(const FKeyEvent &InKeyEvent);

	/// <summary>
	/// Sends the Audio Buffer to the Transcription Service.
	/// </summary>
	/// <param name="AudioBufferToTranscribe">- The Audiobuffer To Send For Transcription.</param>
	void TranscribeWaveForm(TArray<float> AudioBufferToTranscribe);

private:
	/// <summary>
	/// Builds the Phrase Tree.
	/// </summary>
	void BuildPhraseTree();

	/// <summary>
	/// Registers the Console Commands for This Module.
	/// </summary>
	void RegisterConsoleCommands();

	/// <summary>
	/// Unregisters any Console Commands Registered by This Module.
	/// </summary>
	void UnregisterConsoleCommands();


public:
	/// <summary>
	/// A Delegate for when Transcriptions are recived back from the Transcription Service.
	/// </summary>
	TMulticastDelegate<void(TArray<FString>)> OnTranscriptionRecieved;


	/// <summary>
	/// The Socket Communication Server, Managing Socket Communication for the Transcription Service.
	/// </summary>
	TSharedPtr<class FSocketCommunicationServer> SocketServer;

	/// <summary>
	/// The PhraseTree, Containing any Bound Phrase Nodes and Commands to Execute from Transcriptions.
	/// </summary>
	TSharedPtr<FPhraseTree> PhraseTree;

	/// <summary>
	/// Phrase Tree Utility Class, For Dealing With Phrase Tree Function Libraries.
	/// </summary>
	class UPhraseTreeUtils *PhraseTreeUtils;

private:
	/// <summary>
	/// The Previously Recorded Audio Buffer.
	/// </summary>
	TArray<float> PrevAudioBuffer;

	FTickerDelegate TickDelegate;
	FTSTicker::FDelegateHandle TickDelegateHandle;

	FDelegateHandle PhraseTreePhraseRecievedHandle;

	FDelegateHandle KeyDownEventHandle;


	TArray<IConsoleCommand *> ConsoleCommands;
};
