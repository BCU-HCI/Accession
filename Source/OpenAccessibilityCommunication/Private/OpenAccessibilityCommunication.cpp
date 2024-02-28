// Copyright Epic Games, Inc. All Rights Reserved.

#include "OpenAccessibilityCommunication.h"
#include "OpenAccessibilityComLogging.h"

#include "AudioManager.h"

#define LOCTEXT_NAMESPACE "FOpenAccessibilityPythonModule"

void FOpenAccessibilityCommunicationModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogOpenAccessibilityCom, Display, TEXT("OpenAccessibilityComModule::StartupModule()"));

	// Initialize AudioManager
	AudioManager = NewObject<UAudioManager>();
	AudioManager->AddToRoot();

	KeyDownEventHandle = FSlateApplication::Get().OnApplicationPreInputKeyDownListener().AddRaw(this, &FOpenAccessibilityCommunicationModule::HandleKeyDownEvent);
}

void FOpenAccessibilityCommunicationModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UE_LOG(LogOpenAccessibilityCom, Display, TEXT("OpenAccessibilityComModule::ShutdownModule()"));

	AudioManager->RemoveFromRoot();

	FSlateApplication::Get().OnApplicationPreInputKeyDownListener().Remove(KeyDownEventHandle);
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

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FOpenAccessibilityCommunicationModule, OpenAccessibility)