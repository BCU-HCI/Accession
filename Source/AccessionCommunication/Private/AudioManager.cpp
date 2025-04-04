// Copyright F-Dudley. All Rights Reserved.

#include "AudioManager.h"
#include "AccessionComLogging.h"
#include "SocketCommunicationServer.h"

#include "AudioCapture.h"
#include "AudioDeviceNotificationSubsystem.h"
#include "Templates/Function.h"

UAudioManager::UAudioManager(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
	Settings = FAudioManagerSettings();

	// Create Audio Capture Object and Initialize Audio Stream
	bIsCapturingAudio = false;
	AudioCapture = ObjectInitializer.CreateDefaultSubobject<UAudioCapture>(this, TEXT("AccessionAudioCapture"));
	AudioCapture->OpenDefaultAudioStream();
	AudioCapture->StartCapturingAudio();

	RegisterAudioGenerator();

	// Create FileIO Objects
	FileWriter = new Audio::FSoundWavePCMWriter();
}

UAudioManager::~UAudioManager()
{
	UnregisterAudioGenerator();

	AudioCapture->StopCapturingAudio();
	AudioCapture->RemoveFromRoot();

	delete AudioCapture;
	AudioCapture = nullptr;
	delete FileWriter;
	FileWriter = nullptr;
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
	AudioBuffer.Empty();

	bIsCapturingAudio = true;
}

void UAudioManager::StopCapturingAudio()
{
	bIsCapturingAudio = false;

	if (AudioBuffer.Num() == 0)
		return;

	SaveAudioBufferToWAV(Settings.SavePath);

	if (OnAudioReadyForTranscription.ExecuteIfBound(AudioBuffer))
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
