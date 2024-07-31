import gc
from itertools import chain as iter_chain
from multiprocessing import Lock

import numpy as np
import av


try:
    from .Logging import Log
except ImportError:
    from Logging import Log


class AudioResampler:
    """Audio Resampler for Resampling Incoming Audio to the Target Sample Rate. Using FFmpeg for Resampling."""

    def __init__(self, target_sample_rate: int = 16000):
        """Constructor of Audio Resampler Class

        Args:
            target_sample_rate (int, optional): The Target for all incoming resampling requests. Defaults to 16000 (Required by Whisper).
        """

        self._audio_resampler = av.AudioResampler(
            format="s16", layout="mono", rate=target_sample_rate
        )
        self._resample_mutex = Lock()

    def __del__(self):
        """Destructor of Audio Resampler Class.

        Ensures PyAV Resampler Object is Properly Deleted, calling Garbage Collection in the process.
        """

        # Try Deleting the resampler object to cleanly free up memory
        try:
            del self._audio_resampler
        except:
            pass

        try:  # Delete the mutex
            del self._resample_mutex
        except:
            pass

        # Force Garbage Collection, due to resampler not being properly deleted otherwise.
        gc.collect()

    def resample(
        self,
        audio_data: np.ndarray,
        buffer_sample_rate: int = 48000,
        buffer_num_channels: int = 2,
    ) -> np.ndarray:
        """Resamples the Incoming Audio Data to the Classes Assigned Target Sample Rate.

        Args:
            audio_data (np.ndarray): Audio Data to Resample.
            buffer_sample_rate (int, optional): Sample Rate of the Incoming Audio Data. Defaults to 48000.
            buffer_num_channels (int, optional): Number of Channels in the Incoming Audio Data. Defaults to 2 (Stereo).

        Returns:
            np.ndarray: Resampled Version of the Incoming Audio Data.
        """

        audio_data = self._convert_to_s16(audio_data).reshape(-1, 1)

        frame: av.AudioFrame = av.AudioFrame.from_ndarray(
            audio_data.T,
            format="s16",
            layout="stereo" if buffer_num_channels == 2 else "mono",
        )

        frame.sample_rate = buffer_sample_rate

        resampled_frames: list[av.AudioFrame] = []
        with self._resample_mutex:
            resampled_frames = self._audio_resampler.resample(frame)

        return self._convert_to_float32(resampled_frames[0].to_ndarray()).reshape(
            -1,
        )

    def _resample_frame(self, frame: av.AudioFrame) -> list[av.AudioFrame]:
        """Resamples an AudioFrame to the target sample rate.

        Args:
            frame (av.AudioFrame): An AudioFrame to resample.

        Returns:
            list[av.AudioFrame]: A List of Resampled AudioFrames generated from the input frame,
        """
        with self._resample_mutex:
            return self._audio_resampler.resample(frame)

    def _resample_frames(self, frames: list[av.AudioFrame]):
        """Resamples an array of AudioFrames to the target sample rate.

        Args:
            frames (list[av.AudioFrame]): An array of AudioFrames to resample.

        Yields:
            An Array of Generators for the Resampled AudioFrames from the frame inputs.
        """

        for frame in iter_chain(frames, [None]):
            yield from self._audio_resampler.resample(frame)

    def _convert_to_float32(self, audio_data: np.ndarray) -> np.ndarray:
        """Converts the provided audio data to float32 format.

        Args:
            audio_data (np.ndarray): The audio data to convert.

        Raises:
            ValueError: If the data type is not supported.

        Returns:
            np.ndarray: The Input Audio Data in float32 format.
        """

        if audio_data.dtype == np.float32:
            return audio_data

        elif audio_data.dtype == np.int16:
            return audio_data.astype(np.float32) / 32768.0

        else:
            raise ValueError("Unsupported data type")

    def _convert_to_s16(self, audio_data: np.ndarray) -> np.ndarray:
        """Converts the provided audio data to int16 format.

        Args:
            audio_data (np.ndarray): The audio data to convert.

        Raises:
            ValueError: If the data type is not supported.

        Returns:
            np.ndarray: The Input Audio Data in int16 format.
        """

        if audio_data.dtype == np.int16:
            return audio_data

        elif audio_data.dtype == np.float32:
            return (audio_data * 32768.0).astype(np.int16)

        else:
            raise ValueError("Unsupported data type")
