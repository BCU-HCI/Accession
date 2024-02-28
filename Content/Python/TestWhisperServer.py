from typing import Iterable
import zmq
import time
import numpy as np

from faster_whisper import WhisperModel
from faster_whisper.transcribe import Segment

POST_STATUS = False
POST_DEBUG = True


context = zmq.Context()

socket = context.socket(zmq.REP)
socket_context = socket.bind("tcp://127.0.0.1:5555")
print(
    f"|| Socket Context Info: | addr: {socket_context.addr} | kind: {socket_context.kind} ||"
)

poller = zmq.Poller()
poller.register(socket, zmq.POLLIN)

base_model_name = "small"
better_whisper_model_name = "Systran/faster-distil-whisper-small.en"

whisper_model = WhisperModel(
    better_whisper_model_name, device="cuda", local_files_only=False
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

    if len(polled_events) > 0 and polled_events.get(socket) == zmq.POLLIN:
        if POST_STATUS:
            print("|| Socket Event Occurred ||")

        event_handle_time = time.perf_counter()
        recv_message = np.frombuffer(socket.recv(zmq.DONTWAIT), dtype=np.float32)

        print(f"|| Received Message: {recv_message} ||")

        trans_audio = transcribe_audio(recv_message)

        socket.send_multipart(trans_audio)

        if POST_DEBUG:
            print(
                f"|| Event Handle Time: {time.perf_counter() - event_handle_time}s ||"
            )

    else:
        if POST_STATUS:
            print("No Socket Event Occurred")
