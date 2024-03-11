// Copyright Epic Games, Inc. All Rights Reserved.

#include "OpenAccessibilityCommunication.h"
#include "OpenAccessibilityComLogging.h"

#include "AudioManager.h"
#include "SocketCommunicationServer.h"

#include "Containers/Ticker.h"
#include "Interfaces/IPluginManager.h"
#include "HAL/PlatformProcess.h"

#define LOCTEXT_NAMESPACE "UOpenAccessibilityCommunicationModule"

void FOpenAccessibilityCommunicationModule::StartupModule()
{
	LoadZMQDLL();

	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogOpenAccessibilityCom, Display, TEXT("OpenAccessibilityComModule::StartupModule()"));

	// Initialize AudioManager
	AudioManager = NewObject<UAudioManager>();
	AudioManager->AddToRoot();

	// Initialize Socket Server
	SocketServer = MakeShared<FSocketCommunicationServer>();
	// SocketServer->AddToRoot();


	// Bind Tick Event
	TickDelegate = FTickerDelegate::CreateRaw(this, &FOpenAccessibilityCommunicationModule::Tick);
	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(TickDelegate);

	// Bind Input Events
	KeyDownEventHandle = FSlateApplication::Get().OnApplicationPreInputKeyDownListener().AddRaw(this, &FOpenAccessibilityCommunicationModule::HandleKeyDownEvent);
}

void FOpenAccessibilityCommunicationModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UE_LOG(LogOpenAccessibilityCom, Display, TEXT("OpenAccessibilityComModule::ShutdownModule()"));

	AudioManager->RemoveFromRoot();

	FSlateApplication::Get().OnApplicationPreInputKeyDownListener().Remove(KeyDownEventHandle);

	UnloadZMQDLL();

}

bool FOpenAccessibilityCommunicationModule::Tick(const float DeltaTime)
{
	if (SocketServer->EventOccured())
	{
		TArray<FString> RecvStrings;

		if (SocketServer->RecvStringMultipart(RecvStrings))
		{
			UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Tick || Received Multipart | Message Count: %d ||"), RecvStrings.Num());

			for (int i = 0; i < RecvStrings.Num(); i++)
			{
				UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Received Multipart Part: %d | Message: %s ||"), i, *RecvStrings[i]);
			}

			//OnTranscriptionRecieved.ExecuteIfBound(RecvStrings);
		}
	}

	return true;
}

void FOpenAccessibilityCommunicationModule::HandleKeyDownEvent(const FKeyEvent& InKeyEvent)
{
	// If the Space Key is pressed, we will send a request to the Accessibility Server
	if (InKeyEvent.GetKey() == EKeys::SpaceBar)
	{
		if (InKeyEvent.IsShiftDown())
		{
			UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Keydown Event || Shift + SpaceBar ||"));
			AudioManager->StopCapturingAudio();
		}
		else
		{
			UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Keydown Event || SpaceBar ||"));
			AudioManager->StartCapturingAudio();
		}
	}
}

bool FOpenAccessibilityCommunicationModule::TranscribeWaveForm(TArray<float> AudioBufferToTranscribe)
{
	if (AudioBufferToTranscribe.Num() == 0)
	{
		UE_LOG(LogOpenAccessibilityCom, Warning, TEXT("|| Transcription Ready || Audio Buffer is Empty ||"));
		return false;
	}

	UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| WaveForm Transcription || Array Size: %d || Byte Size: %s ||"), AudioBufferToTranscribe.Num(), *FString::FromInt(AudioBufferToTranscribe.Num() * sizeof(float)));

	if (SocketServer->SendArray(AudioBufferToTranscribe.GetData(), AudioBufferToTranscribe.Num(), zmq::send_flags::dontwait))
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Transcription Ready || Sent Audio Buffer ||"));

		return true;
	}

	return false;
}

void FOpenAccessibilityCommunicationModule::LoadZMQDLL()
{
	FString ZMQBinaries = FPaths::Combine(IPluginManager::Get().FindPlugin("OpenAccessibility")->GetBaseDir(), TEXT("Binaries/ThirdParty/ZeroMQ/"));

	FString DllPath;
#if PLATFORM_WINDOWS

	#if UE_BUILD_DEBUG
	DllPath = FPaths::Combine(*ZMQBinaries, TEXT("Win64/libzmq-mt-gd-4_3_5.dll"));
	#else
	DllPath = FPaths::Combine(*ZMQBinaries, TEXT("Win64/libzmq-mt-4_3_5.dll"));
	#endif

#elif PLATFORM_LINUX

	// Not Implemented Yet

#elif PLATFORM_MAC

	// Not Implemented Yet

#endif

	ZMQDllHandle = FPlatformProcess::GetDllHandle(*DllPath);
	if (ZMQDllHandle)
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| LoadZMQDLL || Successfully Loaded ZMQ DLL ||"));
	}
	else
	{
		UE_LOG(LogOpenAccessibilityCom, Error, TEXT("|| LoadZMQDLL || Failed to Load ZMQ DLL ||"));
	}
}

void FOpenAccessibilityCommunicationModule::UnloadZMQDLL()
{
	FPlatformProcess::FreeDllHandle(ZMQDllHandle);
	ZMQDllHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FOpenAccessibilityCommunicationModule, OpenAccessibility)