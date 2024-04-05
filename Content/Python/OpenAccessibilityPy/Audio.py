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

    def __init__(self, target_sample_rate: int = 16000):
        self._audio_resampler = av.AudioResampler(
            format="s16", layout="mono", rate=target_sample_rate
        )
        self._resample_mutex = Lock()

    def __del__(self):
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

    def resample(self, audio_data: np.ndarray, buffer_sample_rate: int) -> np.ndarray:

        audio_data = self._convert_to_s16(audio_data).reshape(-1, 1)

        frame: av.AudioFrame = av.AudioFrame.from_ndarray(
            audio_data.T,
            format="s16",
            layout="stereo",
        )

        frame.sample_rate = buffer_sample_rate

        resampled_frames: list[av.AudioFrame] = []
        with self._resample_mutex:
            resampled_frames = self._audio_resampler.resample(frame)

        return self._convert_to_float32(resampled_frames[0].to_ndarray()).reshape(
            -1,
        )

    def _resample_frames(self, frames):
        for frame in iter_chain(frames, [None]):
            yield from self._audio_resampler.resample(frame)

    def _convert_to_float32(self, audio_data: np.ndarray) -> np.ndarray:

        if audio_data.dtype == np.float32:
            return audio_data

        elif audio_data.dtype == np.int16:
            return audio_data.astype(np.float32) / 32768.0

        else:
            raise ValueError("Unsupported data type")

    def _convert_to_s16(self, audio_data: np.ndarray) -> np.ndarray:

        if audio_data.dtype == np.int16:
            return audio_data

        elif audio_data.dtype == np.float32:
            return (audio_data * 32768.0).astype(np.int16)

        else:
            raise ValueError("Unsupported data type")
