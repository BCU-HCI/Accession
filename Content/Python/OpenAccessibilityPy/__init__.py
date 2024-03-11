import unreal as ue
import zmq
import numpy as np

from .CommunicationServer import CommunicationServer
from .WhisperInterface import WhisperInterface
from .Logging import Log, LogLevel


class OpenAccessibilityPy:
    def __init__(self, whisper_model: str = "base"):
        self.whisper_interface = WhisperInterface(whisper_model)
        self.com_server = CommunicationServer(zmq.REP)

        self.tick_handle = ue.register_slate_post_tick_callback(self.Tick)

    def __del__(self):
        self.Shutdown()

    def Tick(self, delta_time: float):

        if self.com_server.EventOccured():
            Log("Event Occured")

            recv_message = self.com_server.ReceiveNDArray()

            message_ndarray: np.ndarray = np.frombuffer(recv_message, dtype=np.float32)

            Log(
                f"Recieved Message: {message_ndarray} | Size: {message_ndarray.size} | Shape: {message_ndarray.shape}"
            )

            transcription_segments = self.whisper_interface.process_audio_buffer(
                message_ndarray
            )

            encoded_segments = [
                transcription.text.encode() for transcription in transcription_segments
            ]

            mock_transcription = [
                "VIEW NODE 0",
                "NODE 0 MOVE UP 10",
            ]

            mock_encoded_segments = [
                transcription.encode() for transcription in mock_transcription
            ]

            Log(f"Encoded Segments: {encoded_segments}")
            Log(f"Encoded Mock Segments: {mock_encoded_segments}")

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

        if self.com_server:
            del self.com_server

        if self.whisper_interface:
            del self.whisper_interface
