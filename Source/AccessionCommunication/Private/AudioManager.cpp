// Copyright (C) HCI-BCU 2025. All rights reserved.

#include "AudioManager.h"
#include "AccessionComLogging.h"
#include "AccessionCommunicationSettings.h"

#include "AudioCapture.h"
#include "AudioDeviceNotificationSubsystem.h"
#include "Templates/Function.h"

UAudioManager::UAudioManager(const FObjectInitializer& ObjectInitializer)
{
	AudioCapture = ObjectInitializer.CreateDefaultSubobject<UAudioCapture>(this, TEXT("AccessionAudioCapture"));

	Settings = FAudioManagerSettings();
	bIsCapturingAudio = false;
}

UAudioManager::~UAudioManager()
{
	if (AudioCapture->IsValidLowLevel())
	{
		AudioCapture->StopCapturingAudio();
		AudioCapture->RemoveFromRoot();

		
		UnregisterAudioGenerator();
	}

	if (FileWriter != nullptr)
	{
		delete FileWriter;
		FileWriter = nullptr;
	}

}

void UAudioManager::Initialize()
{
	// Ensure Not Default Object.
	if (HasAnyFlags(RF_ClassDefaultObject))
		return;

	FileWriter = new Audio::FSoundWavePCMWriter();

	RegisterAudioGenerator();
}

bool UAudioManager::IsCapturingAudio() const
{
	return bIsCapturingAudio;
}

int32 UAudioManager::GetAudioCaptureSampleRate() const
{
	return AudioCapture->GetSampleRate();
}

int32 UAudioManager::GetAudioCaptureNumChannels() const
{
	return AudioCapture->GetNumChannels();
}

void UAudioManager::StartCapturingAudio()
{
	bIsCapturingAudio = true;
}

void UAudioManager::StopCapturingAudio()
{
	bIsCapturingAudio = false;

	if (AudioBuffer.Num() == 0)
		return;

	const UAccessionCommunicationSettings* ACSettings = GetDefault<UAccessionCommunicationSettings>();

	if (ACSettings && ACSettings->bSaveTemporaryWAV)
		SaveAudioBufferToWAV(Settings.SavePath);

	if (OnAudioReadyForTranscription.ExecuteIfBound(AudioBuffer, AudioCapture->GetSampleRate(), AudioCapture->GetNumChannels()))
	{
		UE_LOG(LogAccessionCom, Log, TEXT("|| Executing Audio Ready For Transcription Delegate. ||"));
	}
	else
	{
		UE_LOG(LogAccessionCom, Warning, TEXT("|| No Delegates Bound to Audio Ready For Transcription Delegate. ||"));
	}

	AudioBuffer.Empty();
}

void UAudioManager::PRIVATE_OnAudioGenerate(const float *InAudio, int32 NumSamples)
{
	if (bIsCapturingAudio == false)
		return;

	// Need to Check Samples are above threshold and ignore if their run length is too long.

	AudioBuffer.Append(InAudio, NumSamples);
}

void UAudioManager::SaveAudioBufferToWAV(const FString &FilePath)
{
	UE_LOG(LogAccessionCom, Log, TEXT("Starting to Save Audio Buffer to WAV"));

	Audio::FSampleBuffer SampleBuffer = Audio::FSampleBuffer(AudioBuffer.GetData(), AudioBuffer.Num(), AudioCapture->GetNumChannels(), AudioCapture->GetSampleRate());

	FileWriter->BeginWriteToWavFile(SampleBuffer, Settings.SaveName, const_cast<FString &>(FilePath), []()
									{ UE_LOG(LogAccessionCom, Log, TEXT("Audio Buffer Saved to WAV")); });
}

void UAudioManager::OnDefaultDeviceChanged(EAudioDeviceChangedRole ChangedRole, FString DeviceID)
{
	UE_LOG(LogAccessionCom, Log, TEXT("|| Default Device Changed || Role: %d || DeviceID: %s ||"), ChangedRole, *DeviceID);

	this->UnregisterAudioGenerator();
	this->RegisterAudioGenerator();
}

void UAudioManager::RegisterAudioGenerator()
{
	// Add Audio Generator Delegate to get audio data from stream,
	// and apply wrapper function due to wanting to reference class function.
	OnAudioGenerateHandle = AudioCapture->AddGeneratorDelegate(FOnAudioGenerate([this](const float *InAudio, int32 NumSamples)
																				{
		if (this->IsCapturingAudio()) this->PRIVATE_OnAudioGenerate(InAudio, NumSamples); }));
}

void UAudioManager::UnregisterAudioGenerator()
{
	AudioCapture->RemoveGeneratorDelegate(OnAudioGenerateHandle);
}
