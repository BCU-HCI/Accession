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

	bool OpenDefaultAudioStream(int32 OverrideSampleRate, int32 OverrideInputChannels);
};
