import unreal
import numpy as np

from faster_whisper import WhisperModel
from faster_whisper.transcribe import Segment


from .Logging import Log, LogLevel


@unreal.uclass()
class WhisperInterface(unreal.Class):

    def __init__(
        self,
        model_name: str = "base",
        device: str = "cuda",
        compute_type: str = "default",
    ):
        # Whisper Focused Variables
        self.whisper_model = WhisperModel(
            model_name, device=device, compute_type=compute_type
        )
        self.beam_size = 5

    def process_file_from_dir(self, filepath: str):

        segments, info = self.whisper_model.transcribe(
            filepath, beam_size=self.beam_size
        )

        Log(
            f"WhisperInterface | Detected Language: {info.language} | Probability: {info.language_probability} | Duration: {info.duration}"
        )

        for segment in segments:
            Log(
                f"WhisperInterface | Segment : {segment.text} | Start: {segment.start} | End: {segment.end}"
            )

        return list(segments)

    def process_audio_buffer(self, audio_buffer: list[float]) -> list[str]:

        audio_buffer: np.ndarray = np.array(audio_buffer, dtype=np.float32)

        segments, info = self.whisper_model.transcribe(
            audio_buffer, beam_size=self.beam_size
        )

        Log(
            f"WhisperInterface | Detected Language: {info.language} | Probability: {info.probability} | Duration: {info.duration}"
        )

        for segment in segments:
            Log(
                "WhisperInterface || Segment: {segment.text} | Start: {segment.start} | End: {segment.end}"
            )

        return list(segments)

    def process_audio_buffer(self, audio_buffer: np.ndarray) -> list[Segment]:

        segments, info = self.whisper_model.transcribe(
            audio_buffer, beam_size=self.beam_size
        )

        Log(
            f"WhisperInterface || Detected Language: {info.language} | Probability: {info.language_probability} | Duration: {info.duration}"
        )

        return list(segments)
