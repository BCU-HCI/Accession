import unreal as ue
import zmq

from .CommunicaitonServer import CommunicationServer
from .WhisperInterface import WhisperInterface


class OpenAccessibilityPy:
    def __init__(self, whisper_model: str = "base"):
        self.whisper_interface = WhisperInterface(whisper_model)
        self.com_server = CommunicationServer(zmq.REP)

        self.tick_handle = ue.register_slate_post_tick_callback(self.Tick)

    def __del__(self):
        self.Shutdown()

    def Tick(self, delta_time: float):

        if self.com_server.EventOccured():
            ue.log("|| Event Occurred ||")

            message_ndarray = self.com_server.ReceiveNDArray()

            transcribed_audio = [
                segment.text.encode()
                for segment in self.whisper_interface.process_audio_buffer(
                    message_ndarray
                )
            ]

            self.com_server.SendMultipart(transcribed_audio)

    def Shutdown(self):
        if self.tick_handle:
            ue.unregister_slate_post_tick_callback(self.tick_handle)
            del self.tick_handle

        if self.com_server:
            del self.com_server

        if self.whisper_interface:
            del self.whisper_interface
