import sys
import os


def append_paths_to_library_path(paths: list[str]):
    sys.path.extend(paths)


def get_path_list() -> list[str]:
    return os.getenv("PATH").split(";")


def get_filtered_path_list(filter_list: list[str]) -> list[str]:
    return [
        path for path in get_path_list() for filter in filter_list if filter in path
    ]


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
        if root[len(path) :].count(os.sep) < depth
    ]
