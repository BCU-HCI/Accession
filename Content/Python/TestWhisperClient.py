import zmq
import time
import numpy as np

from faster_whisper import decode_audio

POST_STATUS = False
POST_DEBUG = True

context = zmq.Context()

push_socket = context.socket(zmq.PUSH)
push_socket_context = push_socket.connect("tcp://127.0.0.1:5555")

pull_socket = context.socket(zmq.PULL)
pull_socket_context = pull_socket.bind("tcp://127.0.0.1:5556")

print(f"|| Sockets Opened ||")

poller = zmq.Poller()
poller.register(pull_socket, zmq.POLLIN)

count_time = time.perf_counter()

while True:

    if (time.perf_counter() - count_time) > 3:

        count_time = time.perf_counter()

        print("|| Sending Message ||")

        audio_send_time = time.perf_counter()
        audio: np.ndarray = decode_audio(
            "D:/dev/Unreal Engine/AccessibilityProject/Saved/BouncedWavFiles/OpenAccessibility/Audioclips/Captured_User_Audio.wav"
        )

        push_socket.send(audio)

        if POST_DEBUG:
            print(f"|| Audio Send Time: {time.perf_counter() - audio_send_time}s ||")

    if POST_STATUS:
        print("|| Waiting for Message ||")
    polled_events = dict(poller.poll(10))

    if len(polled_events) > 0 and polled_events.get(pull_socket) == zmq.POLLIN:
        if POST_STATUS:
            print("|| Socket Event Occurred ||")

        recv_message = pull_socket.recv_multipart(zmq.DONTWAIT)

        print(f"|| Received Reply: | {recv_message} | ||\n\n")
    else:
        if POST_STATUS:
            print("No Socket Event Occurred")
