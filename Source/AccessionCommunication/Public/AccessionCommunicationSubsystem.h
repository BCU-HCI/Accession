// Copyright (C) HCI-BCU 2025. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"

#include "AccessionCommunicationSubsystem.generated.h"


DECLARE_DYNAMIC_DELEGATE_RetVal_ThreeParams(FGuid, FTranscribeRequestDelegate, const TArray<float>, AudioData, const int32, SampleRate, const int32, NumChannels);


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTranscriptionReceived, const FString&, RecvTranscription);


UCLASS()
class ACCESSIONCOMMUNICATION_API UAccessionCommunicationSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:

	UAccessionCommunicationSubsystem();
	virtual ~UAccessionCommunicationSubsystem() override;

	// Begin USubsystem

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;

	// End USubsystem


	// Transcription Pipeline

	UFUNCTION(BlueprintCallable, Category = "Accession|Transcription")
	void RequestTranscription(const TArray<float> AudioData, int32 SampleRate, int32 NumChannels);

	UFUNCTION(BlueprintCallable, Category="Accession|Transcription")
	void TranscriptionComplete(FGuid UUID, const FString& Transcription);

	UPROPERTY()
	FTranscribeRequestDelegate OnTranscriptionRequest;

	UPROPERTY()
	FTranscriptionReceived OnTranscriptionReceived;

	// -----

	void HandleKeyDownEvent(const FKeyEvent& InKeyEvent);


private:

	void ProcessPendingTranscriptions();

private:

	FDelegateHandle KeyDownEventHandle;


	UPROPERTY()
	TObjectPtr<class UAudioManager> AudioManager;

	TQueue<FGuid> PendingTranscriptions;
	TSet<FGuid> ActiveTranscriptions;
	TMap<FGuid, FString> TranscriptionStore;
};
