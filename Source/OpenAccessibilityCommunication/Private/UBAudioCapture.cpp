// Copyright F-Dudley. All Rights Reserved.


#include "UBAudioCapture.h"

UBAudioCapture::UBAudioCapture() : UAudioCapture()
{

}

UBAudioCapture::~UBAudioCapture()
{
}

bool UBAudioCapture::OpenDefaultAudioStream(int32 OverrideSampleRate, int32 OverrideInputChannels)
{
	if (!AudioCapture.IsStreamOpen())
	{
		if (!AudioCapture.IsStreamOpen())
		{
			Audio::FOnAudioCaptureFunction OnCapture = [this](const void* AudioData, int32 NumFrames, int32 InNumChannels, int32 InSampleRate, double StreamTime, bool bOverFlow)
				{
					OnGeneratedAudio((const float*)AudioData, NumFrames * InNumChannels);
				};

			// Start the stream here to avoid hitching the audio render thread. 
			Audio::FAudioCaptureDeviceParams Params;
			if (OverrideSampleRate != NULL)
				Params.SampleRate = OverrideSampleRate;
			if (OverrideInputChannels != NULL)
				Params.NumInputChannels = OverrideInputChannels;


			if (AudioCapture.OpenAudioCaptureStream(Params, MoveTemp(OnCapture), 1024))
			{
				// If we opened the capture stream succesfully, get the capture device info and initialize the UAudioGenerator
				Audio::FCaptureDeviceInfo Info;
				if (AudioCapture.GetCaptureDeviceInfo(Info))
				{
					Init(
						OverrideSampleRate != NULL ? OverrideSampleRate : Info.PreferredSampleRate , 
						OverrideInputChannels != NULL ? OverrideInputChannels : Info.InputChannels
					);

					return true;
				}
			}
		}

		return false;
	}

	return false;
}
