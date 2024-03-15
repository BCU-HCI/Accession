from typing import Iterable
import zmq
import time
import numpy as np
import threading

from faster_whisper import WhisperModel
from faster_whisper.transcribe import Segment

POST_STATUS = False
POST_DEBUG = True


context = zmq.Context()

pull_socket = context.socket(zmq.PULL)
pull_socket_context = pull_socket.bind("tcp://127.0.0.1:5555")

push_socket = context.socket(zmq.PUSH)
push_socket_context = push_socket.connect("tcp://127.0.0.1:5556")

poller = zmq.Poller()
poller.register(pull_socket, zmq.POLLIN)

print(f"|| Sockets Opened ||")

base_model_name = "small"
better_whisper_model_name = "Systran/faster-distil-whisper-small.en"

whisper_model = WhisperModel(
    better_whisper_model_name, device="cuda", local_files_only=False
)


def handle_audio_event():
    if POST_STATUS:
        print("|| Socket Event Occurred ||")

    event_handle_time = time.perf_counter()

    recv_message = np.frombuffer(pull_socket.recv(zmq.DONTWAIT), dtype=np.float32)

    print(f"|| Received Message: {recv_message} ||")

    trans_audio = transcribe_audio(recv_message)

    if len(trans_audio) > 0:
        try:
            push_socket.send_multipart(trans_audio)
        except:
            print("|| ERROR SENDING AUDIO ||")
    else:
        print("|| SENDING REPLACEMENT AUDIO ||")

        push_socket.send_multipart(
            [
                "VIEW NODE 0".encode(),
                "NODE 0 MOVE UP 50".encode(),
            ]
        )

    if POST_DEBUG:
        print(
            f"|| Event Handle Time: {time.perf_counter() - event_handle_time}s ||\n\n"
        )


def transcribe_audio(audio: np.ndarray):

    start_time = time.perf_counter()

    segments, _ = whisper_model.transcribe(audio, beam_size=5)

    if POST_DEBUG:
        print(f"|| Transcription Time: {time.perf_counter() - start_time}s ||")

    return [segment.text.encode() for segment in segments]


while True:

    if POST_STATUS:
        print("|| Waiting for Message ||")
    polled_events = dict(poller.poll(10))

    if len(polled_events) > 0 and polled_events.get(pull_socket) == zmq.POLLIN:

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
