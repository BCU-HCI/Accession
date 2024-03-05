import unreal
import numpy as np
import json
import zmq

from .Logging import Log, LogLevel


class CommunicationServer:

    def __init__(
        self,
        socket_type: int,
        socket_addr: str = "tcp://127.0.0.1:5555",
        poll_timeout: int = 10,
    ):
        # Create the Context
        self.context = zmq.Context()

        # Create a Socket
        self.socket = self.context.socket(socket_type)
        self.socket_context = self.socket.bind(socket_addr)

        self.poller = zmq.Poller()
        self.poller.register(self.socket, zmq.POLLIN)
        self.poller_timeout_time = poll_timeout

    def __del__(self):
        self.socket.close()
        self.context.term()

    def EventOccured(self) -> bool:

        polled_events = dict(self.poller.poll(self.poller_timeout_time))
        if len(polled_events) > 0 and polled_events.get(self.socket) == zmq.POLLIN:
            return True
        else:
            return False

    def SendString(self, message: str, flags=0) -> bool:
        try:
            self.socket.send_string(message)
            return True
        except:
            Log("CommunicationServer | Error Sending String Message", LogLevel.WARNING)
            return False

    def SendJSON(self, message: dict) -> bool:
        try:
            self.socket.send_json(message)
            return True
        except:
            Log(
                "CommunicationServer | Error Sending JSON Message",
                LogLevel.WARNING,
            )
            return False

    def SendNDArray(self, message: np.ndarray) -> bool:
        try:
            self.socket.send(message)
            return True
        except:
            Log(
                "CommunicationServer | Error Sending NDArray Message",
                LogLevel.WARNING,
            )
            return False

    def SendMultipart(self, message: list) -> bool:
        try:
            self.socket.send_multipart(message)
            return True
        except:
            unreal.log_warning(
                "CommunicationServer | Error Sending Multipart Message",
                LogLevel.WARNING,
            )
            return False

    def ReceiveString(self) -> str:

        recv_message = self.socket.recv_string(zmq.DONTWAIT)

        return recv_message

    def ReceiveJSON(self):

        recv_message = json.loads(self.socket.recv_json(zmq.DONTWAIT))

        return recv_message

    def ReceiveNDArray(self):

        recv_message = np.frombuffer(self.socket.recv(zmq.DONTWAIT), dtype=np.float32)

        return recv_message

    def ReceiveMultipart(self):

        recv_message = self.socket.recv_multipart(zmq.DONTWAIT)

        return recv_message
