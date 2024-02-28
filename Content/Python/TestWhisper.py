import numpy as np
from faster_whisper import WhisperModel
from faster_whisper.audio import decode_audio
import time


class ModelInfo:
    name: str
    time_to_load: float
    time_to_transcribe: float
    time_total: float


def test_whisper_model(model_name: str, audiobuffer) -> ModelInfo:

    model_info = ModelInfo()
    model_info.name = model_name

    # ---------------------------------
    # Model Initialization
    # ---------------------------------

    start_time = time.perf_counter()

    whisper_model = WhisperModel(model_name, device="cuda", compute_type="default")

    end_time = time.perf_counter()

    model_info.time_to_load = end_time - start_time

    # ---------------------------------
    # Audio Transcription
    # ---------------------------------

    start_time = time.perf_counter()

    segments, _ = whisper_model.transcribe(audiobuffer, beam_size=5)

    end_time = time.perf_counter()

    model_info.time_to_transcribe = end_time - start_time

    model_info.time_total = model_info.time_to_load + model_info.time_to_transcribe

    # ---------------------------------
    # Show Segments
    # ---------------------------------

    for segment in segments:
        print(
            f"|| WhisperInterface || Start: {segment.start} | End: {segment.end} | Text: {segment.text} ||"
        )

    # ---------------------------------

    print(
        f"\n{model_info.name}:\nTime To Load: {model_info.time_to_load}\nTime To Transcribe: {model_info.time_to_transcribe}\nTotal Time: {model_info.time_total}\n"
    )

    return model_info


# ---------------------------------
# Testing Here
# ---------------------------------

filepath = "..\..\..\..\Saved\BouncedWavFiles\OpenAccessibility\Audioclips\Captured_User_Audio.wav"

models_to_test = [
    "tiny",
    "base",
    "small",
]

audiobuffer = decode_audio(filepath)

user_input = input(
    "Model To Test:\n 1. Tiny\n 2. Base\n 3. Small\n Leave Empty to Test All:\n"
).lower()

if user_input == "":

    print(f"Testing All Models")

    for model in models_to_test:
        info = test_whisper_model(model, audiobuffer)

else:
    if models_to_test.__contains__(user_input):
        test_whisper_model(user_input, audiobuffer)
