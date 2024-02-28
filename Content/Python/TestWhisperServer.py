from typing import Iterable
import zmq
import time
import numpy as np

from faster_whisper import WhisperModel
from faster_whisper.transcribe import Segment

context = zmq.Context()

socket = context.socket(zmq.REP)
socket_context = socket.bind("tcp://127.0.0.1:5555")
print(
    f"|| Socket Context Info: | addr: {socket_context.addr} | kind: {socket_context.kind} ||"
)

poller = zmq.Poller()
poller.register(socket, zmq.POLLIN)

whisper_model = WhisperModel(
    "base",
    device="cuda",
)


def transcribe_audio(audio: np.ndarray):

    segments, _ = whisper_model.transcribe(audio, beam_size=5)

    return [segment.text.encode() for segment in segments]


while True:

    print("Polling")
    polled_events = dict(poller.poll(500))

    if len(polled_events) > 0 and polled_events.get(socket) == zmq.POLLIN:
        print("|| Socket Event Occurred ||")

        recv_message = np.frombuffer(socket.recv(zmq.DONTWAIT), dtype=np.float32)

        print(f"|| Received Message: {recv_message} ||")

        trans_audio = transcribe_audio(recv_message)

        socket.send_multipart(trans_audio)
    else:
        print("No Socket Event Occurred")
