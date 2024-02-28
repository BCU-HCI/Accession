import zmq
import time
import numpy as np

from faster_whisper import WhisperModel, decode_audio

context = zmq.Context()

socket = context.socket(zmq.REQ)
socket_context = socket.connect("tcp://127.0.0.1:5555")

poller = zmq.Poller()
poller.register(socket, zmq.POLLIN)

count_time = time.perf_counter()

while True:

    if (time.perf_counter() - count_time) > 3:

        count_time = time.perf_counter()
        try:
            print("|| Sending Message ||")
            audio: np.ndarray = decode_audio(
                "..\..\..\..\Saved\BouncedWavFiles\OpenAccessibility\Audioclips\Captured_User_Audio.mp3"
            )

            socket.send(audio)
        except:
            print("|| Error Sending Message ||")

    print("Polling")
    polled_events = dict(poller.poll(500))

    if len(polled_events) > 0 and polled_events.get(socket) == zmq.POLLIN:
        print("|| Socket Event Occurred ||")

        recv_message = socket.recv(zmq.DONTWAIT)

        print(f"|| Received Reply: | {recv_message} | ||")
    else:
        print("No Socket Event Occurred")
