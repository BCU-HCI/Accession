import os
import re


def get_path_list() -> list[str]:
    """Gets a list of paths in the PATH environment variable.

    Returns:
        list[str]: _description_
    """

    path = os.getenv("PATH", "")
    print(f"Current PATH Env: {path}")

    return path.split(";")


def get_filtered_path_list(filter_list: list[str]) -> list[str]:
    """Gets a list of paths in the PATH environment variable that contain any of the given filters.

    Args:
        filter_list (list[str]): List of Filter Strings to Search for in the PATH env.

    Returns:
        list[str]: List of Found Paths.
    """

    return [p for p in get_path_list() if any(k in p.upper() for k in filter_list)]


def forward_target_files_to_path(
    paths: list[str],
    target_files=re.compile(".*", re.IGNORECASE),
    depth: int = 0,
):
    print(f"Forward Base Paths: {paths}")

    found_paths = set()

    for base_path in paths:
        if not os.path.isdir(base_path):
            continue

        for root, dirs, files in os.walk(base_path, topdown=True):
            # Limit depth
            walk_depth = root[len(base_path) :].count(os.sep)
            if walk_depth >= depth:
                print(f"Skipping {root} due to depth limit.")
                continue

            for file in files:
                if target_files.match(file):
                    print(
                        f"--- Found Target File: {file} at: {os.path.join(root, file)}"
                    )

                    found_paths.add(root)

                    try:
                        os.add_dll_directory(root)
                        os.environ["PATH"] = root + os.pathsep + os.environ["PATH"]
                    except Exception as e:
                        print(f"Error registering {root}: {e}")

    return found_paths
