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

filepath = "D:\dev\Unreal Engine\AccessibilityProject\Plugins\OpenAccessibility\Saved\BouncedWavFiles\OpenAccessibility\Audioclips\Captured_User_Audio.wav"

models_to_test = ["tiny", "base", "small", "Systran/faster-distil-whisper-small.en"]

audiobuffer = decode_audio(filepath)

input_help = "\n"
for index, model in enumerate(models_to_test):
    input_help += f"{index}: {model}\n"

user_input = input(f"Models: {input_help}\nor Leave Empty to Test All:\n").lower()

if user_input == "":

    print(f"Testing All Models")

    for model in models_to_test:
        info = test_whisper_model(model, audiobuffer)

        print(
            f"Model: {info.name} | Time To Load: {info.time_to_load} | Time To Transcribe: {info.time_to_transcribe} | Total Time: {info.time_total}"
        )

else:
    if models_to_test.__contains__(user_input):
        test_whisper_model(user_input, audiobuffer)

    elif user_input.isdigit() and int(user_input) < len(models_to_test):
        test_whisper_model(models_to_test[int(user_input)], audiobuffer)
