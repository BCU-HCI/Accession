from typing import Iterable
import zmq
import time
import numpy as np
import threading

from faster_whisper import WhisperModel
from faster_whisper.transcribe import decode_audio

from OpenAccessibilityPy.CommunicationServer import CommunicationServer
from OpenAccessibilityPy.WhisperInterface import WhisperInterface

## DEBUGGING VARIABLES
POST_STATUS = False
POST_DEBUG = True

COMPARE_DEBUG = True

## CORE VARIABLES
com_server = CommunicationServer()
whisper_interface = WhisperInterface()

# ----- Debug Functions -----


def get_wav_audio(file_path: str) -> np.ndarray:
    return decode_audio(file_path)


def perform_debug_compares(wav_audio: np.ndarray, recv_message: np.ndarray):
    print("|| Performing Debug Compares ||")

    isSame: bool = np.array_equal(wav_audio, recv_message)
    isClose: bool = np.allclose(wav_audio, recv_message)

    print(f"|| Size Comparisons | Is Same: {isSame} | Is Close: {isClose} ||")

    try:
        difference = np.subtract(wav_audio, recv_message)

        print(f"|| Val Difference: {difference} ||")
    except:
        print("|| ERROR PERFORMING DIFFERENCE ||")


# ------ Log Functions ------


def transcribe_audio(audio: np.ndarray):

    start_time = time.perf_counter()

    segments, _ = whisper_interface.transcribe(audio, beam_size=5)

    if POST_DEBUG:
        print(f"|| Transcription Time: {time.perf_counter() - start_time}s ||")

    return [segment.text.encode() for segment in segments]


def handle_audio_event():
    if POST_STATUS:
        print("|| Socket Event Occurred ||")

    # Debug Components
    wav_audio = get_wav_audio(
        "D:/dev/Unreal Engine/AccessibilityProject/Saved/BouncedWavFiles/OpenAccessibility/Audioclips/CAPTURED_USER_AUDIO.wav"
    )

    # ----- #

    event_handle_time = time.perf_counter()

    recv_message = com_server.ReceiveNDArray()

    print(f"|| Received Message: {recv_message} ||")

    trans_audio = transcribe_audio(recv_message)

    if len(trans_audio) > 0:
        try:
            com_server.SendMultipart(trans_audio)
        except:
            print("|| ERROR SENDING AUDIO ||")
    else:
        print("|| SENDING REPLACEMENT AUDIO ||")

        com_server.SendMultipart(
            [
                "VIEW NODE 0".encode(),
                "NODE 0 MOVE UP 50".encode(),
            ]
        )

    # ------------------------------------------------
    if POST_DEBUG:
        print(
            f"|| Event Handle Time: {time.perf_counter() - event_handle_time}s ||\n\n"
        )

    if COMPARE_DEBUG:
        perform_debug_compares(wav_audio, recv_message)


def get_wav_audio(file_path: str) -> np.ndarray:
    return decode_audio(file_path)


print("|| Starting Whisper Server ||")
while True:

    if POST_STATUS:
        print("|| Waiting for Message ||")

    if com_server.EventOccured():

        test_thread = threading.Thread(
            name="Whisper_Transcription_Event",
            target=handle_audio_event,
            daemon=True,
        )

        test_thread.start()

        test_thread.join()

    else:
        if POST_STATUS:
            print("No Socket Event Occurred")
