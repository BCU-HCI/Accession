import numpy as np
from zmq import PUSH as zmq_PUSH, PULL as zmq_PULL

from faster_whisper.transcribe import decode_audio

from CommunicationServer import CommunicationServer
from WhisperInterface import WhisperInterface

from Logging import Log, LogLevel


def main():

    whisper_interface = WhisperInterface("Systran/faster-distil-whisper-small.en")
    com_server = CommunicationServer(
        send_socket_type=zmq_PUSH, recv_socket_type=zmq_PULL, poll_timeout=10
    )

    should_run = True

    print("Starting Run Loop")

    while should_run:

        if com_server.EventOccured():
            recv_message = com_server.ReceiveNDArray()

            message_ndarray: np.ndarray = np.frombuffer(recv_message, dtype=np.float32)

            decoded_ndarray = decode_audio(
                "D:/dev/Unreal Engine/AccessibilityProject/Saved/BouncedWavFiles/OpenAccessibility/Audioclips/Captured_User_Audio.wav"
            )

            isSame = np.array_equal(message_ndarray, decoded_ndarray)

            # difference = np.subtract(message_ndarray, decoded_ndarray)

            Log(
                f"Buffer Comparisons:\n    Original: {message_ndarray} | Shape: {message_ndarray.shape}\n    Decoded: {decoded_ndarray} | Shape: {message_ndarray.shape}\n   Is Same: {isSame}"
            )

            Log(
                f"Recieved Message: {message_ndarray} | Size: {message_ndarray.size} | Shape: {message_ndarray.shape}"
            )

            transcription_segments = whisper_interface.process_audio_buffer(
                decoded_ndarray
            )

            encoded_segments = [
                transcription.text.encode() for transcription in transcription_segments
            ]

            mock_encoded_segments = [
                "VIEW NODE 0".encode(),
                "NODE 0 MOVE UP 50".encode(),
            ]

            Log(f"Encoded Segments: {encoded_segments}")
            Log(f"Encoded Mock Segments: {mock_encoded_segments}")

            if len(encoded_segments) > 0:
                try:
                    com_server.SendMultipart(encoded_segments)
                except:
                    Log("Error Sending Encoded Transcription Segments", LogLevel.ERROR)
            else:
                Log("No Transcription Segments Returned", LogLevel.WARNING)


if __name__ == "__main__":
    main()
