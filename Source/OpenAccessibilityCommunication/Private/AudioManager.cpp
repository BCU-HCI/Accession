// Copyright F-Dudley. All Rights Reserved.

#include "AudioManager.h"
#include "OpenAccessibilityCommunication.h"
#include "OpenAccessibilityComLogging.h"
#include "SocketCommunicationServer.h"

#include "Templates/Function.h"

UAudioManager::UAudioManager()
{
	Settings = FAudioManagerSettings();

	// Create Audio Capture Object and Initialize Audio Stream
	bIsCapturingAudio = false;
    AudioCapture = NewObject<UAudioCapture>();
	AudioCapture->AddToRoot();
	AudioCapture->OpenDefaultAudioStream();
	AudioCapture->StartCapturingAudio();

	// Create FileIO Objects
	FileWriter = new Audio::FSoundWavePCMWriter();

	// Add Audio Generator Delegate to get audio data from stream, 
	// and apply wrapper function due to wanting to reference class function.
	OnAudioGenerateHandle = AudioCapture->AddGeneratorDelegate(FOnAudioGenerate([this](const float* InAudio, int32 NumSamples) { 
		if (this->IsCapturingAudio()) this->PRIVATE_OnAudioGenerate(InAudio, NumSamples);
	}));
}

UAudioManager::~UAudioManager()
{

	AudioCapture->StopCapturingAudio();
	AudioCapture->RemoveGeneratorDelegate(OnAudioGenerateHandle);
	AudioCapture->RemoveFromRoot();

	delete AudioCapture; AudioCapture = nullptr;
	delete FileWriter; FileWriter = nullptr;
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

	SaveAudioBufferToWAV(Settings.SavePath);
	
	SendBufferForTranscription();

	AudioBuffer.Reset();
}

void UAudioManager::PRIVATE_OnAudioGenerate(const float* InAudio, int32 NumSamples)
{
	if (bIsCapturingAudio == false)
		return;

	// Need to Check Samples are above threshold and ignore if their run length is too long.

	AudioBuffer.Append(InAudio, NumSamples);
}

void UAudioManager::SaveAudioBufferToWAV(const FString& FilePath)
{
	UE_LOG(LogOpenAccessibilityCom, Log, TEXT("Starting to Save Audio Buffer to WAV"));

	Audio::FSampleBuffer SampleBuffer = Audio::FSampleBuffer(AudioBuffer.GetData(), AudioBuffer.Num(), AudioCapture->GetNumChannels(), AudioCapture->GetSampleRate());

	FileWriter->BeginWriteToWavFile(SampleBuffer, Settings.SaveName, const_cast<FString&>(FilePath), []() {
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("Audio Buffer Saved to WAV"));
	});
}

void UAudioManager::SendBufferForTranscription()
{
	/*
	if (OnAudioReadyForTranscription.ExecuteIfBound(AudioBuffer))
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Sending Buffer for Transcription | Using Bound Transcriber ||"));
	}
	else
	{
		UE_LOG(LogOpenAccessibilityCom, Warning, TEXT("No Bound Delegates for OnAudioReadyForTranscription"));
	}
	*/

	if (FOpenAccessibilityCommunicationModule::Get()->SocketServer->SendArray(AudioBuffer.GetData(), AudioBuffer.Num(), zmq::send_flags::dontwait))
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Sending Buffer for Transcription | Using Socket Server ||"));
	}
	else
	{
		UE_LOG(LogOpenAccessibilityCom, Warning, TEXT("|| Sending Buffer for Transcription | Failed to Send using Socket Server ||"));
	}
}
