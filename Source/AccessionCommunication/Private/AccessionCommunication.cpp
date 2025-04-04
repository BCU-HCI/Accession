// Copyright Epic Games, Inc. All Rights Reserved.

#include "AccessionCommunication.h"
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
	LoadZMQDLL();

	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogAccessionCom, Display, TEXT("AccessionComModule::StartupModule()"));

	// Initialize AudioManager
	AudioManager = NewObject<UAudioManager>();
	AudioManager->AddToRoot();

	AudioManager->OnAudioReadyForTranscription
		.BindRaw(this, &FAccessionCommunicationModule::TranscribeWaveForm);

	// Initialize Socket Server
	SocketServer = MakeShared<FSocketCommunicationServer>();

	// Build The Phrase Tree
	BuildPhraseTree();

	// Bind Tick Event
	TickDelegate = FTickerDelegate::CreateRaw(this, &FAccessionCommunicationModule::Tick);
	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(TickDelegate);

	// Bind Input Events
	KeyDownEventHandle = FSlateApplication::Get().OnApplicationPreInputKeyDownListener().AddRaw(this, &FAccessionCommunicationModule::HandleKeyDownEvent);

	// Register Console Commands
	RegisterConsoleCommands();
}

void FAccessionCommunicationModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UE_LOG(LogAccessionCom, Display, TEXT("AccessionComModule::ShutdownModule()"));

	AudioManager->RemoveFromRoot();
	PhraseTreeUtils->RemoveFromRoot();

	FSlateApplication::Get().OnApplicationPreInputKeyDownListener().Remove(KeyDownEventHandle);

	UnloadZMQDLL();

	UnregisterConsoleCommands();
}

bool FAccessionCommunicationModule::Tick(const float DeltaTime)
{
	// Detect if any events are ready to be received.
	if (SocketServer->EventOccured())
	{
		TArray<FString> RecvStrings;
		TSharedPtr<FJsonObject> RecvMetadata;

		// Receive the Detected Event, with separate transcriptions and metadata.
		if (SocketServer->RecvStringMultipartWithMeta(RecvStrings, RecvMetadata))
		{
			OA_LOG(LogAccessionCom, Log, TEXT("TRANSCRIPTION RECEIVED"), TEXT("Received Multipart - Message Count: %d"), RecvStrings.Num());
			for (int i = 0; i < RecvStrings.Num(); i++)
			{
				OA_LOG(LogAccessionCom, Log, TEXT("TRANSCRIPTION RECEIVED"), TEXT("Transcription - %d: { %s }"), i, *RecvStrings[i]);
			}

			// Send Received Transcriptions to any bound events.
			OnTranscriptionRecieved.Broadcast(RecvStrings);
		}
	}

	return true;
}

void FAccessionCommunicationModule::HandleKeyDownEvent(const FKeyEvent &InKeyEvent)
{
	FKey EventKey = InKeyEvent.GetKey();

	bool isTargetKey = EventKey == EKeys::LeftAlt || EventKey == EKeys::RightAlt;

	// If the Voice Command Key is Pressed, Toggle Audio Capture.
	if (isTargetKey && !InKeyEvent.IsRepeat())
	{
		if (!AudioManager->IsCapturingAudio())
		{
			OA_LOG(LogAccessionCom, Log, TEXT("AudioCapture Change"), TEXT("Starting Audio Capture"));
			AudioManager->StartCapturingAudio();
		}
		else
		{
			OA_LOG(LogAccessionCom, Log, TEXT("AudioCapture Change"), TEXT("Stopping Audio Capture"));
			AudioManager->StopCapturingAudio();
		}
	}
}

void FAccessionCommunicationModule::TranscribeWaveForm(const TArray<float> AudioBufferToTranscribe)
{
	if (AudioBufferToTranscribe.Num() == 0)
	{
		UE_LOG(LogAccessionCom, Warning, TEXT("|| Transcription Ready || Audio Buffer is Empty ||"));
		return;
	}

	PrevAudioBuffer = TArray(AudioBufferToTranscribe);

	UE_LOG(LogAccessionCom, Log, TEXT("|| WaveForm Transcription || Array Size: %d || Byte Size: %s ||"), AudioBufferToTranscribe.Num(), *FString::FromInt(AudioBufferToTranscribe.Num() * sizeof(float)));

	// Create Metadata of Audio Source.
	TSharedPtr<FJsonObject> AudioBufferMetadata = MakeShared<FJsonObject>();
	AudioBufferMetadata->SetNumberField(TEXT("sample_rate"), AudioManager->GetAudioCaptureSampleRate());
	AudioBufferMetadata->SetNumberField(TEXT("num_channels"), AudioManager->GetAudioCaptureNumChannels());

	bool bArrayMessageSent = SocketServer->SendArrayMessageWithMeta(AudioBufferToTranscribe, AudioBufferMetadata.ToSharedRef(), ComSendFlags::none);

	OA_LOG(LogAccessionCom, Log, TEXT("TRANSCRIPTION SENT"), TEXT("{%s} Send Audiobuffer (float x %d / %d Hz / %d channels)"),
		   bArrayMessageSent ? TEXT("Success") : TEXT("Failed"),
		   AudioBufferToTranscribe.Num(), AudioManager->GetAudioCaptureSampleRate(), AudioManager->GetAudioCaptureNumChannels());
}

void FAccessionCommunicationModule::BuildPhraseTree()
{
	// Initialize the Phrase Tree
	PhraseTree = MakeShared<FPhraseTree>();
	PhraseTreePhraseRecievedHandle = OnTranscriptionRecieved
										 .AddRaw(PhraseTree.Get(), &FPhraseTree::ParseTranscription);

	PhraseTreeUtils = NewObject<UPhraseTreeUtils>();
	PhraseTreeUtils->SetPhraseTree(PhraseTree.ToSharedRef());
	PhraseTreeUtils->AddToRoot();
}

void FAccessionCommunicationModule::RegisterConsoleCommands()
{
	// Audio Commands

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("AccessionCom.Debug.ShowAudioSampleRate"),
		TEXT("Logs the Number of Samples being captured, from user input."),

		FConsoleCommandDelegate::CreateLambda([this]()
											  { UE_LOG(LogAccessionCom, Display, TEXT("AccessionCom.Debug.ShowAudioSampleRate | Sample Rate: %d"), this->AudioManager->GetAudioCaptureSampleRate()); })));

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("AccessionCom.Debug.ShowAudioNumChannels"),
		TEXT("Logs the Number of Audio Channels being captured, from user input."),

		FConsoleCommandDelegate::CreateLambda([this]()
											  { UE_LOG(LogAccessionCom, Display, TEXT("AccessionCom.Debug.ShowAudioNumChannels | Num Channels: %d"), this->AudioManager->GetAudioCaptureNumChannels()); })));

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("AccessionCom.Debug.SendLastBuffer"),
		TEXT("Sends the last saved audio buffer to the transcription service."),

		FConsoleCommandDelegate::CreateLambda([this]()
											  {
			UE_LOG(LogAccessionCom, Display, TEXT("AccessionCom.Debug.SendLastBuffer"));

			TranscribeWaveForm(PrevAudioBuffer); })));
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

void FAccessionCommunicationModule::LoadZMQDLL()
{
	FString BaseDir = IPluginManager::Get().FindPlugin("Accession")->GetBaseDir();

	FString LibraryPath;
#if PLATFORM_WINDOWS
#if UE_BUILD_DEBUG
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/ZeroMQ/Win64/libzmq-mt-gd-4_3_5.dll"));
#else
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/ZeroMQ/Win64/libzmq-mt-4_3_5.dll"));
#endif
#elif PLATFORM_LINUX
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/ZeroMQ/Linux/libzmq-mt-4_3_5.so"))
#elif PLATFORM_MAC
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/ZeroMQ/Mac/libzmq-mt-4_3_5.dylib"))
#endif

	ZMQDllHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

	if (ZMQDllHandle)
	{
		UE_LOG(LogAccessionCom, Log, TEXT("|| LoadZMQDLL || Successfully Loaded ZMQ DLL ||"));
	}
	else
	{
		UE_LOG(LogAccessionCom, Error, TEXT("|| LoadZMQDLL || Failed to Load ZMQ DLL ||"));
	}
}

void FAccessionCommunicationModule::UnloadZMQDLL()
{
	FPlatformProcess::FreeDllHandle(ZMQDllHandle);
	ZMQDllHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAccessionCommunicationModule, AccessionCommunication)