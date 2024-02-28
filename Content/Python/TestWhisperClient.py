import zmq
import time
import numpy as np

from faster_whisper import WhisperModel, decode_audio

POST_STATUS = False
POST_DEBUG = True

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

            audio_send_time = time.perf_counter()
            audio: np.ndarray = decode_audio(
                "..\..\..\..\Saved\BouncedWavFiles\OpenAccessibility\Audioclips\Captured_User_Audio.mp3"
            )

            socket.send(audio)
            if POST_DEBUG:
                print(
                    f"|| Audio Send Time: {time.perf_counter() - audio_send_time}s ||"
                )
        except:
            print("|| Error Sending Message ||")

    if POST_STATUS:
        print("|| Waiting for Message ||")
    polled_events = dict(poller.poll(10))

    if len(polled_events) > 0 and polled_events.get(socket) == zmq.POLLIN:
        if POST_STATUS:
            print("|| Socket Event Occurred ||")

        recv_message = socket.recv(zmq.DONTWAIT)

        print(f"|| Received Reply: | {recv_message} | ||\n\n")
    else:
        if POST_STATUS:
            print("No Socket Event Occurred")
