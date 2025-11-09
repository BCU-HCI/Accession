// Copyright (C) HCI-BCU 2025. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"

#include "AccessionCommunicationSubsystem.generated.h"


class UAudioManager;

DECLARE_DYNAMIC_DELEGATE_RetVal_ThreeParams(FGuid, FTranscribeRequestDelegate, const TArray<float>, AudioData, const int32, SampleRate, const int32, NumChannels);


DECLARE_MULTICAST_DELEGATE_OneParam(FTranscriptionReceived, TArray<FString> /* RecvTranscription*/ );


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

	UAudioManager* GetAudioManager() const
	{
		return AudioManager.Get();
	}

	UFUNCTION(BlueprintCallable, Category = "Accession|Transcription")
	void RequestTranscription(const TArray<float> AudioData, int32 SampleRate, int32 NumChannels);

	UFUNCTION(BlueprintCallable, Category="Accession|Transcription")
	void TranscriptionComplete(const FGuid UUID, const TArray<FString> Transcription);

	UPROPERTY(BlueprintReadWrite, Category = "Accession|Transcription")
	FTranscribeRequestDelegate OnTranscriptionRequest;

	//UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Accession|Transcription")
	FTranscriptionReceived OnTranscriptionReceived;

	// -----

	virtual bool Tick(float DeltaTime);

	void HandleKeyDownEvent(const FKeyEvent& InKeyEvent);


private:

	void ProcessPendingTranscriptions();

public:

	TSharedPtr<class FPhraseTree> PhraseTree;
	
	UPROPERTY()
	TObjectPtr<class UPhraseTreeUtils> PhraseTreeUtils;

private:

	FTSTicker::FDelegateHandle TickDelegateHandle;
	FDelegateHandle KeyDownEventHandle;


	UPROPERTY()
	TObjectPtr<UAudioManager> AudioManager;


	// Transcription Buffering

	TQueue<FGuid> PendingTranscriptions;

	UPROPERTY()
	TSet<FGuid> ActiveTranscriptions;

	TMap<FGuid, TArray<FString>> TranscriptionStore;



	UPROPERTY()
	TArray<float> PrevAudioBuffer;
};
