// Copyright (C) HCI-BCU 2025. All rights reserved.

#include "AccessionCommunicationSubsystem.h"
#include "AccessionAnalytics.h"
#include "AudioManager.h"
#include "AccessionComLogging.h"

#include "PhraseTree.h"
#include "PhraseTreeUtils.h"

#include "Containers/Queue.h"


UAccessionCommunicationSubsystem::UAccessionCommunicationSubsystem()
{
	AudioManager = NewObject<UAudioManager>();
	AudioManager->OnAudioReadyForTranscription.BindUObject(this, &UAccessionCommunicationSubsystem::RequestTranscription);

	PhraseTree = MakeShared<FPhraseTree>();
	//OnTranscriptionReceived.AddSP(PhraseTree.ToSharedRef(), &FPhraseTree::ParseTranscription);

	PhraseTreeUtils = NewObject<UPhraseTreeUtils>();
	PhraseTreeUtils->SetPhraseTree(PhraseTree.ToSharedRef());


	const FTickerDelegate TickDelegate = FTickerDelegate::CreateUObject(this, &UAccessionCommunicationSubsystem::Tick);
	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(TickDelegate);

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

	TranscriptionStore.Add(id, Transcription);
	ActiveTranscriptions.Remove(id);
}

void UAccessionCommunicationSubsystem::ProcessPendingTranscriptions()
{
	if (PendingTranscriptions.IsEmpty())
		return;

	FGuid NextID;
	while (PendingTranscriptions.Peek(NextID))
	{
		if (ActiveTranscriptions.Contains(NextID))
			break;

		PendingTranscriptions.Pop();

		TArray<FString> Transcription = TranscriptionStore.FindAndRemoveChecked(NextID);

		OnTranscriptionReceived.Broadcast(Transcription);
	}
}

bool UAccessionCommunicationSubsystem::Tick(float DeltaTime)
{
	ProcessPendingTranscriptions();

	return true;
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
