from ctypes import Union
import numpy as np

from faster_whisper import WhisperModel
from faster_whisper.transcribe import Segment

try:
    from .Logging import Log, LogLevel
except ImportError:
    from Logging import Log, LogLevel


class WhisperInterface:
    """Interface Class for Interacting with the CTranslate2 Faster Whisper Model."""

    def __init__(
        self,
        model_name: str = "distil-small.en",
        device: str = "auto",
        cpu_threads: int = 4,
        transcribe_workers: int = 2,
        compute_type: str = "default",
    ):
        """Constructor of Whisper Interface Class

        Args:
            model_name (str, optional): Hugging-Face Model Specifier for CTranslate Compatible Models. Defaults to "distil-small.en".
            device (str, optional): Device host for the Whisper Model (Can be "auto", "cpu", "cuda"). Defaults to "auto".
            cpu_threads (int, optional): Amount of CPU Threads to use, if Hosting the Model on a CPU. Defaults to 4.
            transcribe_workers (int, optional): Amount of Thread Workers for Audio Transcription. Defaults to 2.
            compute_type (str, optional): Data Structure for Whisper Compute. Defaults to "default".
        """

        # Whisper Focused Variables
        self.whisper_model = WhisperModel(
            model_name,
            device=device,
            compute_type=compute_type,
            num_workers=transcribe_workers,
            cpu_threads=cpu_threads,
            local_files_only=True,
        )
        self.beam_size = 5

    def __del__(self):
        """Destructor of Whisper Interface Class."""

        del self.whisper_model

    def process_file_from_dir(self, filepath: str):
        """Transcribes an Audio Files from a Given WAV File Path.

        Args:
            filepath (str): Path to the Audio Files to Transcribe.

        Returns:
            A List of Segments containing the Transcribed Text and their Time Stamps.
        """

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
        """Transcribes an NDArray AudioBuffer.

        Args:
            audio_buffer (np.ndarray): AudioBuffer to Transcribe.

        Returns:
            tuple[list[Segment], dict]: Tuple Containing a List of Transcription Segments and a Dictionary of Collected Metadata.
        """

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
