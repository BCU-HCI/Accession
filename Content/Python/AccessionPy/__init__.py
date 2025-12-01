import unreal as ue
from unreal import GuidLibrary
import numpy as np
from gc import collect as gc_collect

from concurrent.futures import ThreadPoolExecutor as ThreadPool

from .WhisperInterface import WhisperInterface
from .Audio import AudioResampler
from .TranscriptionBuffer import TranscriptionBuffer
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


class AccessionPy:
    """Python Runtime Class for Accession Plugin"""

    def __init__(
        self,
        # General Runtime Specifics
        worker_count: int = 2,
        # Whisper Specifics
        whisper_model: str = "distil-small.en",
        device: str = "auto",
        compute_type: str = "default",
    ):
        """Constructor of Python Runtime Class for Accession Plugin

        Args:
            worker_count (int, optional): Amount of Thread Workers for Audio Transcription. Defaults to 2.
            whisper_model (str, optional): Hugging-Face Model Specifier for CTranslate Compatible Models. Defaults to "distil-small.en".
            device (str, optional): Device host for the Whisper Model (Can be "auto", "cpu", "cuda"). Defaults to "auto".
            compute_type (str, optional): Data Structure for Whisper Compute. Defaults to "default".
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

        self.audio_resampler = AudioResampler(target_sample_rate=16000)

        self.store = TranscriptionBuffer()

        self.tick_handle = ue.register_slate_post_tick_callback(self.Tick)

        self.pyshutdown_handle = ue.register_python_shutdown_callback(self.Shutdown)

        self.acsubsystem = ue.get_editor_subsystem(ue.AccessionCommunicationSubsystem)
        if not self.acsubsystem:
            Log("Accession Communication Subsystem Not Found", LogLevel.ERROR)
            exit(1)

        self.acsubsystem.on_transcription_request.bind_callable(
            self.HandleTranscriptionRequest
        )

    def __del__(self):
        """Destructor of Python Runtime Class for Accession Plugin"""

        self.Shutdown()

    def Tick(self, delta_time: float):
        """Tick Callback for Unreal Engine Slate Post Tick.

        Handling Completed Transcriptions, and Forwarding back to the Communication Subsystem.

        Args:
            delta_time (float): Time since last tick
        """

        self.store.process_completed(self.acsubsystem.transcription_complete)

    def HandleTranscriptionRequest(
        self, recv_buffer: ue.Array[float], sample_rate: int, num_channels: int
    ):

        message_ndarray = np.array(recv_buffer, dtype=np.float32)

        generated_id = GuidLibrary.new_guid()

        self.worker_pool.submit(
            self._handle_transcription_request,
            generated_id,
            message_ndarray,
            sample_rate,
            num_channels,
        )

        self.store.register_active(generated_id)

        return generated_id

    def _handle_transcription_request(
        self,
        id: ue.Guid,
        recv_buffer: ue.Array[float],
        sample_rate: int,
        num_channels: int,
    ):

        Log(
            f"Handling Transcription Request | Buffer Size: {len(recv_buffer)} | Sample Rate: {sample_rate} | Num Channels: {num_channels}"
        )

        recv_buffer = np.array(recv_buffer, dtype=np.float32).reshape(1, -1)

        message_ndarray = self.audio_resampler.resample(
            recv_buffer, sample_rate, num_channels
        )

        trans_segment, trans_metadata = self.whisper_interface.process_audio_buffer(
            message_ndarray
        )

        segments = [transcription.text.strip() for transcription in trans_segment]

        Log(f"Segments: {segments}")

        try:
            self.store.store_completed(id, segments)
        except:
            Log("Error With Transcription Segments", LogLevel.ERROR)

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
