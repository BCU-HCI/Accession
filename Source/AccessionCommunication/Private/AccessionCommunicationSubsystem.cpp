#include "AccessionCommunicationSubsystem.h"
#include "Containers/Queue.h"

#include "AccessionComLogging.h"

UAccessionCommunicationSubsystem::UAccessionCommunicationSubsystem()
{
	
}

UAccessionCommunicationSubsystem::~UAccessionCommunicationSubsystem()
{
}

void UAccessionCommunicationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
}

void UAccessionCommunicationSubsystem::Deinitialize()
{
	
}

void UAccessionCommunicationSubsystem::RequestTranscription(const TArray<float>& AudioData, int32 SampleRate, int32 NumChannels)
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
}

void UAccessionCommunicationSubsystem::TranscriptionComplete(const FGuid id, const FString& Transcription)
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

		FString Transcription = TranscriptionStore.FindAndRemoveChecked(NextID);

		OnTranscriptionReceived.Broadcast(Transcription);
	}
}
