import os
import sys
import re
import numpy as np
import time

from faster_whisper import WhisperModel
from faster_whisper.transcribe import Segment

DEVICE_TYPES = ["cpu", "cuda"]

TEST_CONFIG = {
    "inference_iterations": 3,  # Number of Transcription Iterations to Test for Each Device
    "duration": 5,  # Duration of the Dummy Audio in Seconds
    "sample_rate": 16000,  # Sample Rate for the Dummy Audio
}


def load_model(device: str = "auto", compute_type: str = "default") -> WhisperModel:

    model = WhisperModel(
        "distil-small.en",
        device=device,
        compute_type=compute_type,
        num_workers=2,
        cpu_threads=8,
    )

    return model


def get_child_directories(path: str, depth: int = 0) -> list[str]:
    """
    Recursively searches the given directory, for any further child directories.

    Args:
        path (str): The path to the directory to search.
        depth (int): The depth to search for child directories. Defaults to 0.
    """

    assert os.path.isdir(path), f"Path: {path} is not a directory."

    return [
        root
        for root, _, _ in os.walk(path, topdown=True)
        if root[len(path) :].count(os.sep) <= depth
    ]


def forward_cuda_cudnn_to_path():
    """
    Finds cuDNN 9 subdirectories and registers them.
    Also checks for the existence of the specific ops DLL.
    """
    path_list = os.getenv("PATH", "").split(";")
    filter_keywords = ["CUDA", "CUDNN"]

    # Track if we actually found the 'ops' DLL
    found_ops_dll = False

    for base_path in path_list:
        if not any(
            k in base_path.upper() for k in filter_keywords
        ) or not os.path.isdir(base_path):
            continue

        # Search this path and 1 level deep for the DLLs
        for root, dirs, files in os.walk(base_path):
            # Limit depth to 1 level below bin
            depth = root[len(base_path) :].count(os.sep)
            if depth > 1:
                continue

            if "cudnn_ops64_9.dll" in files:
                found_ops_dll = True
                print(f"--- Found cuDNN Ops at: {root}")

            # Register every directory that looks like a bin or versioned bin
            try:
                os.add_dll_directory(root)
                # Also extend PATH for older compatibility / subprocesses
                os.environ["PATH"] = root + os.pathsep + os.environ["PATH"]
            except Exception as e:
                print(f"Error registering {root}: {e}")

    if not found_ops_dll:
        print("!!! WARNING: cudnn_ops64_9.dll was NOT found in your PATH folders.")
        print(
            "Ensure cuDNN 9 is installed and its 'bin' folder (and subfolders) are in Windows PATH."
        )


def forward_cuda_cudnn_to_path_re():
    dll_regex = re.compile(r"cudnn_.*64_.*\.dll", re.IGNORECASE)

    # Get Base PATH
    path = os.getenv("PATH", "")
    path = path.split(";")

    # Filter Base PATH for CUDA/CUDNN Folders
    filter_keywords = ["CUDA", "CUDNN"]
    candidate_paths = [p for p in path if any(k in p.upper() for k in filter_keywords)]

    found_paths = set()

    for base_path in candidate_paths:
        if not os.path.isdir(base_path):
            continue

        for root, dirs, files in os.walk(base_path):
            depth = root[len(base_path) :].count(os.sep)
            if depth > 1:
                continue

            dll_dir = False

            for file in files:
                if dll_regex.match(file):
                    print(f"--- Found cuDNN DLL at: {os.path.join(root, file)}")
                    dll_dir = True
                    break

            if dll_dir:
                try:
                    found_paths.add(root)

                    os.add_dll_directory(root)
                    os.environ["PATH"] = root + os.pathsep + os.environ["PATH"]
                except Exception as e:
                    print(f"Error registering {root}: {e}")

    print(
        f"Forwarded CUDA/CUDNN Paths to System PATH. ({len(found_paths)} directories registered)",
        found_paths,
    )


if __name__ == "__main__":

    print("Forwarding CUDA and CUDNN Paths to System Path...")
    forward_cuda_cudnn_to_path()

    print("Testing Whisper Model Loading and Transcription...")

    dummy_audio = np.random.rand(
        TEST_CONFIG["duration"] * TEST_CONFIG["sample_rate"]
    ).astype(np.float32)

    for device in DEVICE_TYPES:
        print(f"-- Testing on Device: {device} --")

        print("Loading Model...")
        model = load_model(device=device, compute_type="default")
        print("Model Loaded Successfully.")

        # --- #

        print("Testing Transcription...")

        for i in range(TEST_CONFIG["inference_iterations"]):
            print(f"Iteration {i + 1} of {TEST_CONFIG['inference_iterations']}")

            iter_time = time.perf_counter_ns()

            segments, info = model.transcribe(
                dummy_audio,
                beam_size=5,
            )

            print(
                f"Transcription Info | Language: {info.language} | Duration: {info.duration:.2f}s"
            )

            print(
                "Duration Matches Expected: ",
                info.duration - TEST_CONFIG["duration"] == 0,
            )

            print(f"Segments Detected: {segments}", "Iterating Segments...")

            for idx, segment in enumerate(segments):
                print(
                    f"Iter{i + 1} | Segment {idx} | Text: {segment.text} | Start: {segment.start:.2f}s | End: {segment.end:.2f}s"
                )

            iter_time = time.perf_counter_ns() - iter_time
            print(
                f"Iteration {i + 1} completed in {iter_time / 1e6:.2f}ms ({iter_time / 1e9:.2f}s)"
            )

        print(f"{device} Transcription Test Completed.\n")
        print(f"{'-' * 40}\n")
