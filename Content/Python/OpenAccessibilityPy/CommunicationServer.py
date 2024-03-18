import numpy as np
import json
import zmq

try:
    from .Logging import Log, LogLevel
except ImportError:
    from Logging import Log, LogLevel


class CommunicationServer:

    def __init__(
        self,
        send_socket_type: int,
        recv_socket_type: int,
        send_socket_addr: str = "tcp://127.0.0.1:5556",
        recv_socket_addr: str = "tcp://127.0.0.1:5555",
        poll_timeout: int = 10,
    ):
        # Create the Context
        self.context = zmq.Context()

        # Create a Socket
        self.send_socket: zmq.Socket = self.context.socket(send_socket_type)
        self.send_socket_context = self.send_socket.connect(send_socket_addr)

        self.recv_socket = self.context.socket(recv_socket_type)
        self.recv_socket_context = self.recv_socket.bind(recv_socket_addr)

        self.poller = zmq.Poller()
        self.poller.register(self.recv_socket, zmq.POLLIN)
        self.poller_timeout_time = poll_timeout

    def __del__(self):

        self.send_socket.close()
        self.recv_socket.close()

        self.context.term()

    def EventOccured(self) -> bool:

        polled_events = dict(self.poller.poll(self.poller_timeout_time))
        if len(polled_events) > 0 and polled_events.get(self.recv_socket) == zmq.POLLIN:
            return True
        else:
            return False

    def SendString(self, message: str, flags=0) -> bool:
        try:
            self.send_socket.send_string(message)
            return True
        except:
            Log("CommunicationServer | Error Sending String Message", LogLevel.WARNING)
            return False

    def SendJSON(self, message: dict) -> bool:
        try:
            self.send_socket.send_json(message)
            return True
        except:
            Log(
                "CommunicationServer | Error Sending JSON Message",
                LogLevel.WARNING,
            )
            return False

    def SendNDArray(self, message: np.ndarray) -> bool:
        try:
            self.send_socket.send(message)
            return True
        except:
            Log(
                "CommunicationServer | Error Sending NDArray Message",
                LogLevel.WARNING,
            )
            return False

    def SendMultipart(self, message: list) -> bool:
        try:
            self.send_socket.send_multipart(message)
            return True
        except:
            Log(
                "CommunicationServer | Error Sending Multipart Message",
                LogLevel.WARNING,
            )
            return False

    def ReceiveString(self) -> str:

        return self.recv_socket.recv_string(zmq.DONTWAIT)

    def ReceiveJSON(self):

        return json.loads(self.recv_socket.recv_json(zmq.DONTWAIT))

    def ReceiveNDArray(self):

        return np.frombuffer(self.recv_socket.recv(zmq.DONTWAIT), dtype=np.float32)

    def ReceiveMultipart(self):

        return self.recv_socket.recv_multipart(zmq.DONTWAIT)
