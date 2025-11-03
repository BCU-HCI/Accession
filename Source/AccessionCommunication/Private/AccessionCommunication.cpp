// Copyright (C) HCI-BCU 2025. All rights reserved.

#include "AccessionCommunication.h"
#include "AccessionCommunicationSubsystem.h"
#include "AccessionComLogging.h"

#include "AccessionAnalytics.h"

#include "AudioManager.h"
#include "SocketCommunicationServer.h"

#include "PhraseTree/PhraseNode.h"
#include "PhraseTree/PhraseInputNode.h"
#include "PhraseTree/PhraseDirectionalInputNode.h"
#include "PhraseTree/PhraseEventNode.h"

#include "Containers/Ticker.h"
#include "Dom/JsonObject.h"
#include "Interfaces/IPluginManager.h"
#include "Sound/SampleBufferIO.h"
#include "HAL/PlatformProcess.h"

#define LOCTEXT_NAMESPACE "FAccessionCommunicationModule"

void FAccessionCommunicationModule::StartupModule()
{

	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogAccessionCom, Display, TEXT("AccessionComModule::StartupModule()"));

	//AudioManager->OnAudioReadyForTranscription
	//	.BindRaw(this, &FAccessionCommunicationModule::TranscribeWaveForm);

	// Initialize Socket Server
	//SocketServer = MakeShared<FSocketCommunicationServer>();

	// Build The Phrase Tree
	BuildPhraseTree();

	// Register Console Commands
	RegisterConsoleCommands();
}

void FAccessionCommunicationModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UE_LOG(LogAccessionCom, Display, TEXT("AccessionComModule::ShutdownModule()"));

	PhraseTreeUtils->RemoveFromRoot();

	UnregisterConsoleCommands();
}

void FAccessionCommunicationModule::BuildPhraseTree()
{
	

	// Initialize the Phrase Tree
	PhraseTree = MakeShared<FPhraseTree>();

	UAccessionCommunicationSubsystem* ACSubsystem = GEditor->GetEditorSubsystem<UAccessionCommunicationSubsystem>();
	if (ACSubsystem != nullptr)
	{
		// Require Phrase Tree Rewrite towards UObject Incorporation.
		// PhraseTreePhraseRecievedHandle = ACSubsystem->OnTranscriptionReceived.AddUnique(PhraseTree.Get(), &FPhraseTree::ParseTranscription);
	}


	PhraseTreeUtils = NewObject<UPhraseTreeUtils>();
	PhraseTreeUtils->SetPhraseTree(PhraseTree.ToSharedRef());
	PhraseTreeUtils->AddToRoot();
}

void FAccessionCommunicationModule::RegisterConsoleCommands()
{

}

void FAccessionCommunicationModule::UnregisterConsoleCommands()
{
	IConsoleCommand *ConsoleCommand = nullptr;
	while (ConsoleCommands.Num() > 0)
	{
		ConsoleCommand = ConsoleCommands.Pop();

		IConsoleManager::Get().UnregisterConsoleObject(ConsoleCommand);
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAccessionCommunicationModule, AccessionCommunication)