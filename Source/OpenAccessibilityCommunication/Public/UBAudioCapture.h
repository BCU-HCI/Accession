// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AudioCapture.h"

/**
 * 
 */
class OPENACCESSIBILITYCOMMUNICATION_API UBAudioCapture : public UAudioCapture
{

public:
	UBAudioCapture();
	~UBAudioCapture();

	/// <summary>
	/// Opens the default audio stream.
	/// </summary>
	/// <param name="OverrideSampleRate">Override for the Audiobuffers Sample Rate.</param>
	/// <param name="OverrideInputChannels">Override for the Amount of Input Channel Amount.</param>
	/// <returns>True, if the Audiostream was opened correctly. False, if the Audio Stream could not be opened.</returns>
	bool OpenDefaultAudioStream(int32 OverrideSampleRate, int32 OverrideInputChannels);
};
