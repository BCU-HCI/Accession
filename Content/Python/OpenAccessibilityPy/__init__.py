import unreal as ue
import zmq
import numpy as np
from gc import collect as gc_collect

from concurrent.futures import ThreadPoolExecutor as ThreadPool

from .CommunicationServer import CommunicationServer
from .WhisperInterface import WhisperInterface
from .Audio import AudioResampler
from .Logging import Log, LogLevel

from .LibUtils import (
    get_filtered_path_list,
    get_child_directories,
    append_paths_to_library_path,
)


def forward_CUDA_CUDNN_to_path():
    """
    Forces any found CUDA and CUDNN Paths to the System Path.

    This is useful for circumventing issues with CUDA and CUDNN not being found on the embedded interpreter.
    Not always needed, but useful for some systems.
    """

    filtered_path_list = get_filtered_path_list(["CUDA", "CUDNN"])

    for path in filtered_path_list:
        append_paths_to_library_path(get_child_directories(path, depth=1))


class OpenAccessibilityPy:
    """Python Runtime Class for Open Accessbility Plugin"""

    def __init__(
        self,
        # General Runtime Specifics
        worker_count: int = 2,
        # Whisper Specifics
        whisper_model: str = "distil-small.en",
        device: str = "auto",
        compute_type: str = "default",
        # Communication Specifics
        poll_timeout: int = 10,
    ):
        """Constructor of Python Runtime Class for Open Accessibility Plugin

        Args:
            worker_count (int, optional): Amount of Thread Workers for Audio Transcription. Defaults to 2.
            whisper_model (str, optional): Hugging-Face Model Specifier for CTranslate Compatible Models. Defaults to "distil-small.en".
            device (str, optional): Device host for the Whisper Model (Can be "auto", "cpu", "cuda"). Defaults to "auto".
            compute_type (str, optional): Data Structure for Whisper Compute. Defaults to "default".
            poll_timeout (int, optional): Amount of time (ms) for event polling on the Transcription Socket. Defaults to 10.
        """

        self.worker_pool = ThreadPool(
            max_workers=worker_count, thread_name_prefix="TranscriptionWorker"
        )

        self.whisper_interface = WhisperInterface(
            model_name=whisper_model,
            device=device,
            compute_type=compute_type,
            transcribe_workers=worker_count,
        )
        self.com_server = CommunicationServer(
            send_socket_type=zmq.PUSH,
            recv_socket_type=zmq.PULL,
            poll_timeout=poll_timeout,
        )
        self.audio_resampler = AudioResampler(target_sample_rate=16000)

        self.tick_handle = ue.register_slate_post_tick_callback(self.Tick)

        self.pyshutdown_handle = ue.register_python_shutdown_callback(self.Shutdown)

    def __del__(self):
        """Destructor of Python Runtime Class for Open Accessibility Plugin"""

        self.Shutdown()

    def Tick(self, delta_time: float):
        """Tick Callback for Unreal Engine Slate Post Tick.

        Detecting Incoming Transcription Requests and Handling them, through the Worker Pool.

        Args:
            delta_time (float): Time since last tick
        """

        if self.com_server.EventOccured():
            Log("Event Occured")

            message, metadata = self.com_server.ReceiveNDArrayWithMeta(dtype=np.float32)

            self.worker_pool.submit(self.HandleTranscriptionRequest, message, metadata)

    def HandleTranscriptionRequest(
        self, recv_message: np.ndarray, metadata: dict = None
    ):
        """Handles Incoming Transcription Requests

        Takes the Incoming AudioBuffer, Resamples it to 16kHz and Transcribes it using Whisper.

        Args:
            recv_message (np.ndarray): ndarray of the incoming audio buffer.
            metadata (dict, optional): metadata of the incoming audio buffer, if any is recieved. Defaults to None.
        """

        Log(
            f"Handling Transcription Request | Message: {recv_message} | Size: {recv_message.size} | Shape: {recv_message.shape}"
        )

        sample_rate = metadata.get("sample_rate", 48000)
        num_channels = metadata.get("num_channels", 2)

        message_ndarray = self.audio_resampler.resample(
            recv_message, sample_rate, num_channels
        )

        trans_segments, trans_metadata = self.whisper_interface.process_audio_buffer(
            message_ndarray
        )

        encoded_segments = [
            transcription.text.encode() for transcription in trans_segments
        ]

        Log(f"Encoded Segments: {encoded_segments}")

        if len(encoded_segments) > 0:
            try:
                self.com_server.SendMultipartWithMeta(
                    message=encoded_segments, meta=trans_metadata
                )

            except:
                Log("Error Sending Encoded Transcription Segments", LogLevel.ERROR)

        else:
            Log("No Transcription Segments Returned", LogLevel.WARNING)

    def Shutdown(self):
        """Shutsdown the Python Runtime Components, and Forces a Garbage Collection."""

        if self.tick_handle:
            ue.unregister_slate_post_tick_callback(self.tick_handle)
            del self.tick_handle

        if self.worker_pool:
            self.worker_pool.shutdown(wait=False, cancel_futures=True)
            del self.worker_pool

        if self.audio_resampler:
            del self.audio_resampler

        if self.com_server:
            del self.com_server

        if self.whisper_interface:
            del self.whisper_interface

        # Force a Garbage Collection
        gc_collect()
