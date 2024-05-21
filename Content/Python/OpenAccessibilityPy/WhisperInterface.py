from ctypes import Union
import numpy as np

from faster_whisper import WhisperModel
from faster_whisper.transcribe import Segment

try:
    from .Logging import Log, LogLevel
except ImportError:
    from Logging import Log, LogLevel


class WhisperInterface:

    def __init__(
        self,
        model_name: str = "Systran/faster-distil-whisper-small.en",
        device: str = "auto",
        compute_type: str = "default",
    ):
        # Whisper Focused Variables
        self.whisper_model = WhisperModel(
            model_name,
            device=device,
            compute_type=compute_type,
            num_workers=2,
            cpu_threads=4,
            local_files_only=True,
        )
        self.beam_size = 5

    def __del__(self):
        del self.whisper_model

    def process_file_from_dir(self, filepath: str):

        segments, info = self.whisper_model.transcribe(
            filepath,
            beam_size=self.beam_size,
            language="en",
            prepend_punctuations="",
            append_punctuations="",
            vad_filter=True,
        )

        Log(
            f"WhisperInterface | Detected Language: {info.language} | Probability: {info.language_probability} | Duration: {info.duration}"
        )

        for segment in segments:
            Log(
                f"WhisperInterface | Segment : {segment.text} | Start: {segment.start} | End: {segment.end}"
            )

        return list(segments)

    def process_audio_buffer(
        self, audio_buffer: np.ndarray
    ) -> tuple[list[Segment], dict]:

        segments, info = self.whisper_model.transcribe(
            audio_buffer,
            beam_size=self.beam_size,
            language="en",
        )

        Log(
            f"WhisperInterface || Detected Language: {info.language} | Probability: {info.language_probability} | Duration: {info.duration}"
        )

        collected_metadata = {
            "duration": info.duration,
            "language": info.language,
            "language_probability": info.language_probability,
        }

        return list(segments), collected_metadata
