// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "AudioCapture.h"
#include "Sound/SampleBufferIO.h"
#include "Delegates/DelegateCombinations.h"

#include "AudioManager.generated.h"

USTRUCT()
struct FAudioManagerSettings
{
    GENERATED_BODY()

public:
    FAudioManagerSettings()
    {
        // Default Settings
        LevelThreshold = -2.5f;
        SaveName = FString("Captured_User_Audio");
        SavePath = FString("./OpenAccessibility/Audioclips/");
    }

    // The Threshold for incoming audio to be considered as input.
    UPROPERTY(Config, EditAnywhere, Category = "OpenAccessibility/Audio Manager")
    float LevelThreshold;

    /// <summary>
    /// The Name of the Audio File to be saved to.
    /// </summary>
    UPROPERTY(Config, EditAnywhere, Category = "OpenAccessibility/Audio Manager")
    FString SaveName;

    /// <summary>
    /// The Path to save recorded audio files to.
    /// </summary>
    UPROPERTY(Config, EditAnywhere, Category = "OpenAccessibility/Audio Manager")
    FString SavePath;
};


/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, Config = OpenAccessibility)
class OPENACCESSIBILITYCOMMUNICATION_API UAudioManager : public UObject
{
    GENERATED_BODY()

public:
    UAudioManager();
    virtual ~UAudioManager();

    void StartCapturingAudio();
    void StopCapturingAudio();

    void PRIVATE_OnAudioGenerate(const float* InAudio, int32 NumSamples);

    void SaveAudioBufferToWAV(const FString& FilePath);

    bool IsCapturingAudio() const { return bIsCapturingAudio; }

public:
    UPROPERTY(Config, EditAnywhere, Category = "OpenAccessibility/Audio Manager")
    FAudioManagerSettings Settings;

    TDelegate<void(const TArray<float>)> OnAudioReadyForTranscription;

private:
    
    // Audio Capture
    bool bIsCapturingAudio = false;
    UAudioCapture* AudioCapture;
    TArray<float> AudioBuffer;

    FAudioGeneratorHandle OnAudioGenerateHandle;

    // Audio Saving
    Audio::FSoundWavePCMWriter* FileWriter;
};