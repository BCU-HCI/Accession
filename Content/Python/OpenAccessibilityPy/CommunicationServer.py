import numpy as np
import json
import zmq

try:
    from .Logging import Log, LogLevel
except ImportError:
    from Logging import Log, LogLevel


class CommunicationServer:
    """Communication Server Class for Handling Communication Between Python and C++.

    Using ZeroMQ for Socket Communication. (Push / PULL Architecture)
    """

    def __init__(
        self,
        send_socket_type: int,
        recv_socket_type: int,
        send_socket_addr: str = "tcp://127.0.0.1:5556",
        recv_socket_addr: str = "tcp://127.0.0.1:5555",
        poll_timeout: int = 10,
    ):
        """Constructor of Communication Server Class

        Args:
            send_socket_type (int): ZeroMQ Socket Type for Sending Messages.
            recv_socket_type (int): ZeroMQ Socket Type for Receiving Messages.
            send_socket_addr (str, optional): Local Address / Port for Sending Communication Data. Defaults to "tcp://127.0.0.1:5556".
            recv_socket_addr (str, optional): Local Address / Port for Receiving Communication Data. Defaults to "tcp://127.0.0.1:5555".
            poll_timeout (int, optional): Amount of time (ms) for event polling on the Receive Socket. Defaults to 10.
        """

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
        """Destructor of Communication Server Class.

        Closes the Sockets and Terminates the ZeroMQ Context.
        """

        self.send_socket.close()
        self.recv_socket.close()

        self.context.term()

    def EventOccured(self) -> bool:
        """Checks if a Receive Event has Occured on the Receive Socket.

        Returns:
            bool: True if an Event has Occured, False Otherwise.
        """

        polled_events = dict(self.poller.poll(self.poller_timeout_time))
        if len(polled_events) > 0 and polled_events.get(self.recv_socket) == zmq.POLLIN:
            return True
        else:
            return False

    def SendString(self, message: str) -> bool:
        """Sends a String Message on the Send Socket.

        Args:
            message (str): String Message to Send.

        Returns:
            bool: True if the Message was Sent Successfully, False Otherwise.
        """

        try:
            self.send_socket.send_string(message)
            return True
        except:
            Log("CommunicationServer | Error Sending String Message", LogLevel.WARNING)
            return False

    def SendJSON(self, message: dict) -> bool:
        """Sends a JSON Message on the Send Socket.

        Args:
            message (dict): Stringified JSON Message to Send.

        Returns:
            bool: True if the Message was Sent Successfully, False Otherwise.
        """

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
        """Sends a Numpy NDArray Message on the Send Socket.

        Args:
            message (np.ndarray): NDArray of Data to Send.

        Returns:
            bool: True if the Data was Sent Successfully, False Otherwise.
        """

        try:
            self.send_socket.send(message)
            return True
        except:
            Log(
                "CommunicationServer | Error Sending NDArray Message",
                LogLevel.WARNING,
            )
            return False

    def SendNDArrayWithMeta(self, message: np.ndarray, meta: dict) -> bool:
        """Sends a Numpy NDArray Message with Metadata on the Send Socket.

        Args:
            message (np.ndarray): NDArray of Data to Send.
            meta (dict): A Dictionary of Metadata to Send.

        Returns:
            bool: True if the Data was Sent Successfully, False Otherwise.
        """

        try:
            self.send_socket.send_multipart([json.dumps(meta).encode(), message.data])

            return True
        except:
            Log(
                "CommunicationServer | Error Sending NDArray With Meta Message",
                LogLevel.WARNING,
            )
            return False

    def SendMultipart(self, message: list) -> bool:
        """Sends a Multipart Message on the Send Socket.

        Args:
            message (list): List of Messages to Send.

        Returns:
            bool: True if the MultiPart Message was Sent Successfully, False Otherwise.
        """

        try:
            self.send_socket.send_multipart(message)
            return True
        except:
            Log(
                "CommunicationServer | Error Sending Multipart Message",
                LogLevel.WARNING,
            )
            return False

    def SendMultipartWithMeta(self, message: list, meta: dict) -> bool:
        """Sends a Multipart Message with Metadata on the Send Socket.

        Args:
            message (list): List of Messages to Send.
            meta (dict): Metadata to Send.

        Returns:
            bool: True if the MultiPart Message with Metadata was Sent Successfully, False Otherwise.
        """

        try:
            self.send_socket.send_multipart([json.dumps(meta).encode(), *message])
            return True
        except:
            Log(
                "CommunicationServer | Error Sending Multipart With Meta Message",
                LogLevel.WARNING,
            )
            return False

    def RecieveRaw(self):
        """Receives a Raw Message of Bytes from the Receive Socket.

        Returns:
            bytes: Raw Received Bytes from the Receive Socket.
        """

        return self.recv_socket.recv(zmq.DONTWAIT)

    def ReceiveString(self) -> str:
        """Receives a String Message from the Receive Socket.

        Returns:
            str: Received String Message.
        """

        return self.recv_socket.recv_string(zmq.DONTWAIT)

    def ReceiveJSON(self):
        """Receive a JSON Message from the Receive Socket.

        Returns:
            dict: Dictionary of the Received JSON Message.
        """

        return json.loads(self.recv_socket.recv_json(zmq.DONTWAIT))

    def ReceiveNDArray(self, dtype=np.float32) -> np.ndarray:
        """Receives a Numpy NDArray from the Receive Socket.

        Args:
            dtype (optional): Type of NDArray of Received Data. Defaults to np.float32.

        Returns:
            np.ndarray: Receieved NDArray Message.
        """

        return np.frombuffer(
            self.recv_socket.recv(zmq.DONTWAIT),
            dtype=dtype,
        )

    def ReceiveNDArrayWithMeta(self, dtype=np.float32) -> tuple[np.ndarray, dict]:
        """Receives a Numpy NDArray with Metadata from the Receive Socket.

        Args:
            dtype (optional): Type of NDArray of Received Data. Defaults to np.float32.

        Returns:
            tuple[np.ndarray, dict]: Tuple of Received NDArray and Dict Metadata Object.
        """

        recv_message = self.recv_socket.recv_multipart(zmq.DONTWAIT)

        if len(recv_message) > 1:
            return (
                np.frombuffer(recv_message[1], dtype=dtype),
                json.loads(recv_message[0]),
            )

        elif len(recv_message) == 1:
            Log(
                "CommunicationServer | Error Receiving NDArray With Meta. Only Contains One Message, Assumed Data.",
                LogLevel.WARNING,
            )
            return (np.frombuffer(recv_message[0], dtype=dtype), {})

        Log("CommunicationServer | Error Receiving NDArray With Meta", LogLevel.WARNING)

    def ReceiveMultipart(self) -> list[bytes]:
        """Receieved a Raw Multipart Message from the Receive Socket.

        Returns:
            list[bytes]: Raw List of Bytes from the Received Multipart Message.
        """

        return self.recv_socket.recv_multipart(zmq.DONTWAIT)
