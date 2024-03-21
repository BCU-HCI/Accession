import unreal as ue
import zmq
import numpy as np

from multiprocessing.pool import ThreadPool

from .CommunicationServer import CommunicationServer
from .WhisperInterface import WhisperInterface
from .Audio import AudioResampler
from .Logging import Log, LogLevel


# def HandleTranscriptionNewWhisper(com_server: CommunicationServer):
#     whisper_interface = WhisperInterface("Systran/faster-distil-whisper-small.en")

#     recv_message = com_server.ReceiveNDArray()

#     # Get the message as a numpy array, reshaped for a stereo audio buffer.
#     message_ndarray: np.ndarray = np.frombuffer(recv_message, dtype=np.float32)

#     Log(
#         f"Recieved Message: {message_ndarray} | Size: {message_ndarray.size} | Shape: {message_ndarray.shape}"
#     )

#     transcription_segments = whisper_interface.process_audio_buffer(message_ndarray)

#     if len(transcription_segments) > 0:

#         encoded_segments = [
#             transcription.text.encode() for transcription in transcription_segments
#         ]

#         Log(f"Encoded Segments: {encoded_segments}")
#         try:
#             com_server.SendMultipart(encoded_segments)

#         except:
#             Log("Error Sending Encoded Transcription Segments", LogLevel.ERROR)

#     else:
#         Log("No Transcription Segments Returned", LogLevel.WARNING)


class OpenAccessibilityPy:
    def __init__(
        self,
        whisper_model: str = "Systran/faster-distil-whisper-small.en",
        worker_count: int = 2,
    ):
        self.worker_pool = ThreadPool(processes=worker_count)

        self.whisper_interface = WhisperInterface(whisper_model)
        self.com_server = CommunicationServer(
            send_socket_type=zmq.PUSH, recv_socket_type=zmq.PULL, poll_timeout=10
        )
        self.audio_resampler = AudioResampler(sample_rate=16000)

        self.tick_handle = None
        self.tick_handle = ue.register_slate_post_tick_callback(self.Tick)

    def __del__(self):
        self.Shutdown()

    def Tick(self, delta_time: float):

        if self.com_server.EventOccured():
            Log("Event Occured")

            result = self.worker_pool.apply_async(self.HandleTranscriptionRequest)
            Log("|| Transcription Work Requested ||")

            # transcription_thread = Thread(
            #     name="TranscriptionHandlerThread",
            #     target=self.HandleTranscriptionRequest,
            #     daemon=True,
            # )

            # transcription_thread.start()

            # transcription_thread.join()

    def HandleTranscriptionRequest(self):
        recv_message = self.com_server.ReceiveNDArray()

        message_ndarray: np.ndarray = np.frombuffer(recv_message, dtype=np.float32)

        Log(
            f"Recieved Message: {message_ndarray} | Size: {message_ndarray.size} | Shape: {message_ndarray.shape}"
        )

        message_ndarray = self.audio_resampler.resample(message_ndarray)

        transcription_segments = self.whisper_interface.process_audio_buffer(
            message_ndarray
        )

        encoded_segments = [
            transcription.text.encode() for transcription in transcription_segments
        ]

        Log(f"Encoded Segments: {encoded_segments}")

        if len(encoded_segments) > 0:
            try:
                self.com_server.SendMultipart(encoded_segments)

            except:
                Log("Error Sending Encoded Transcription Segments", LogLevel.ERROR)

        else:
            Log("No Transcription Segments Returned", LogLevel.WARNING)

    def Shutdown(self):
        if self.tick_handle:
            ue.unregister_slate_post_tick_callback(self.tick_handle)
            del self.tick_handle

        if self.worker_pool:
            self.worker_pool.close()
            self.worker_pool.terminate()

        if self.audio_resampler:
            del self.audio_resampler

        if self.com_server:
            del self.com_server

        if self.whisper_interface:
            del self.whisper_interface
