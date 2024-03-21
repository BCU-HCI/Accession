import numpy as np
from zmq import PUSH as zmq_PUSH, PULL as zmq_PULL

from faster_whisper.transcribe import decode_audio

from CommunicationServer import CommunicationServer
from WhisperInterface import WhisperInterface
from Audio import AudioResampler

from Logging import Log, LogLevel


def PlotAudioBuffers(
    recv_audio_buffer: np.ndarray,
    decoded_audio_buffer: np.ndarray,
    name: str = "BufferComparison",
):
    """
    Plots the received audio buffer and the decoded audio buffer to compare the two.
    """

    try:
        import matplotlib as mpl
        from matplotlib import pyplot as plt

        mpl.interactive(False)

        fig, axs = plt.subplots(3)

        axs[0].plot(recv_audio_buffer)

        axs[1].plot(decoded_audio_buffer)

        axs[2].plot(recv_audio_buffer)
        axs[2].plot(decoded_audio_buffer)
        axs[2].set_title("Buffer Comparison")

        fig.savefig(
            f"D:/dev/Unreal Engine/AccessibilityProject/Saved/Debug/OpenAccessibility/{name}.png",
            dpi=300,
        )

        fig.clear()

    except Exception as e:
        Log(f"Error Plotting Audio Buffers: {e}", LogLevel.ERROR)


def main():

    whisper_interface = WhisperInterface("Systran/faster-distil-whisper-small.en")
    com_server = CommunicationServer(
        send_socket_type=zmq_PUSH, recv_socket_type=zmq_PULL, poll_timeout=10
    )
    audio_resampler = AudioResampler(sample_rate=16000)

    should_run = True

    print("Starting Run Loop")

    while should_run:

        if com_server.EventOccured():
            Log("Event Occured")

            recv_message = com_server.ReceiveNDArray()

            message_ndarray: np.ndarray = np.frombuffer(recv_message, dtype=np.float32)

            decoded_ndarray = decode_audio(
                "D:/dev/Unreal Engine/AccessibilityProject/Saved/BouncedWavFiles/OpenAccessibility/Audioclips/Captured_User_Audio.wav",
                sampling_rate=16000,
            )

            PlotAudioBuffers(message_ndarray, decoded_ndarray)

            isSame = np.array_equal(message_ndarray, decoded_ndarray)
            # isClose = np.allclose(message_ndarray, decoded_ndarray)

            # difference = np.subtract(message_ndarray, decoded_ndarray)

            Log(f"Recieved Buffer | {message_ndarray} | Shape: {message_ndarray.shape}")

            Log(f"Decoded Buffer | {decoded_ndarray} | Shape: {decoded_ndarray.shape}")
            Log(f"Comparisons | Is Same: {isSame}")

            # Apply Resampling to the Audio Buffer, to match samplerate of 16000Hz
            message_ndarray = audio_resampler.resample(message_ndarray)

            PlotAudioBuffers(
                message_ndarray, decoded_ndarray, name="ResampledBufferComparison"
            )

            transcription_segments = whisper_interface.process_audio_buffer(
                message_ndarray
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
