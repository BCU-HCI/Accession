// Copyright (C) HCI-BCU 2025. All rights reserved.

#include "AccessionCommunicationSubsystem.h"
#include "AccessionAnalytics.h"
#include "AudioManager.h"
#include "AccessionComLogging.h"

#include "Containers/Queue.h"


UAccessionCommunicationSubsystem::UAccessionCommunicationSubsystem()
{
	AudioManager = NewObject<UAudioManager>();
	AudioManager->OnAudioReadyForTranscription.BindUObject(this, &UAccessionCommunicationSubsystem::RequestTranscription);

	KeyDownEventHandle = FSlateApplication::Get().OnApplicationPreInputKeyDownListener().AddUObject(this, &UAccessionCommunicationSubsystem::HandleKeyDownEvent);
}

UAccessionCommunicationSubsystem::~UAccessionCommunicationSubsystem()
{
	FSlateApplication::Get().OnApplicationPreInputKeyDownListener().Remove(KeyDownEventHandle);
}

void UAccessionCommunicationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
}

void UAccessionCommunicationSubsystem::Deinitialize()
{
	
}

void UAccessionCommunicationSubsystem::RequestTranscription(const TArray<float> AudioData, const int32 SampleRate, const int32 NumChannels)
{
	if (!OnTranscriptionRequest.IsBound())
	{
		UE_LOG(LogAccessionCom, Error, TEXT("Transcription Request Delegate Not Bound. No Commands can be Performed."))
		return;
	}

	FGuid TranscriptionUUID = OnTranscriptionRequest.Execute(AudioData, SampleRate, NumChannels);
	if (!TranscriptionUUID.IsValid())
	{
		UE_LOG(LogAccessionCom, Error, TEXT("Transcription Request Delegate Returned Invalid UUID. No Commands can be Performed."))
		return;
	}

	PendingTranscriptions.Enqueue(TranscriptionUUID);
	ActiveTranscriptions.Add(TranscriptionUUID);

	PrevAudioBuffer = AudioData;
}

void UAccessionCommunicationSubsystem::TranscriptionComplete(const FGuid id, const TArray<FString> Transcription)
{
	if (!ActiveTranscriptions.Contains(id))
	{
		UE_LOG(LogAccessionCom, Warning, TEXT("Received Transcription Complete for Unknown UUID: %s"), *id.ToString());
		return;
	}

	if (*PendingTranscriptions.Peek() != id)
	{
		TranscriptionStore.Add(id, Transcription);
		return;
	}

	PendingTranscriptions.Pop();
	ActiveTranscriptions.Remove(id);

	OnTranscriptionReceived.Broadcast(Transcription);

	ProcessPendingTranscriptions();
}

void UAccessionCommunicationSubsystem::ProcessPendingTranscriptions()
{
	FGuid NextID;

	while (PendingTranscriptions.Peek(NextID))
	{
		if (!TranscriptionStore.Contains(NextID))
			break;

		PendingTranscriptions.Pop();
		ActiveTranscriptions.Remove(NextID);

		TArray<FString> Transcription = TranscriptionStore.FindAndRemoveChecked(NextID);

		OnTranscriptionReceived.Broadcast(Transcription);
	}
}

void UAccessionCommunicationSubsystem::HandleKeyDownEvent(const FKeyEvent& InKeyEvent)
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
