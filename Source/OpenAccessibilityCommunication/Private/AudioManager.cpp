// Copyright F-Dudley. All Rights Reserved.


#include "AudioManager.h"
#include "OpenAccessibilityLogging.h"

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
	
	ProcessBufferForTranscription();

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
	UE_LOG(LogOpenAccessibility, Log, TEXT("Starting to Save Audio Buffer to WAV"));

	Audio::FSampleBuffer SampleBuffer = Audio::FSampleBuffer(AudioBuffer.GetData(), AudioBuffer.Num(), AudioCapture->GetNumChannels(), AudioCapture->GetSampleRate());

	FileWriter->BeginWriteToWavFile(SampleBuffer, Settings.SaveName, const_cast<FString&>(FilePath), []() {
		UE_LOG(LogOpenAccessibility, Log, TEXT("Audio Buffer Saved to WAV"));
	});
}

void UAudioManager::ProcessBufferForTranscription()
{
	if (OnAudioReadyForTranscription.IsBound())
	{
		UE_LOG(LogOpenAccessibility, Log, TEXT("Sending Buffer for Transcription"));

		TArray<FString> transcription = OnAudioReadyForTranscription.Execute(AudioBuffer);

		UE_LOG(LogOpenAccessibility, Log, TEXT("Returned Transcription: %s"), *transcription[0]);
	}
	else
	{
		UE_LOG(LogOpenAccessibility, Warning, TEXT("No Bound Delegates for OnAudioReadyForTranscription"));
	}
}
