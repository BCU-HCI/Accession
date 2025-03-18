import unreal

import subprocess
import pkg_resources
from os import path


def get_requirements(requirements_dir: str) -> list[str]:
    with open(
        path.join(requirements_dir, "requirements.txt"), "r"
    ) as requirements_file:
        return [line.strip() for line in requirements_file.readlines()]


def is_dependency_satisfied(dependency: str) -> bool:
    try:
        pkg_resources.require(dependency)
        return True
    except:
        return False


def install_dependencies(deps_to_install: list[str]):

    unreal.log_warning(
        f"|| OpenAccessibility Python || Installing Dependencies: {deps_to_install} ||"
    )

    with unreal.ScopedSlowTask(
        len(deps_to_install),
        "OpenAccessibility Installing Python Dependencies",
        enabled=True,
    ) as install_ui:

        for dep in deps_to_install:

            install_str = f"Installing {dep}"

            print(install_str)
            install_ui.enter_progress_frame(1, install_str)

            process = subprocess.Popen(
                [
                    unreal.get_interpreter_executable_path(),
                    "-m",
                    "pip",
                    "install",
                    "--force-reinstall",  # Force Reinstall to avoid satisfying the requirement from global site-packages.
                    dep,
                ],
                shell=True,
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
            )

            while process.poll() is None:
                install_ui.enter_progress_frame(
                    0, process.stdout.readline().decode("utf-8")
                )

            process.wait()


## ------------------------------

unreal.log("|| OpenAccessibility Python || Initializing ||")

## ------------------------------

# Verify Required Dependencies

missing_deps = [
    dep
    for dep in get_requirements(path.dirname(path.realpath(__file__)))
    if not is_dependency_satisfied(dep)
]

if missing_deps:
    install_dependencies(missing_deps)

## ------------------------------

# Initialize the Python Runtime

unreal.log("|| OpenAccessibility Python || Starting Python Runtime ||")

import OpenAccessibilityPy as OAPy

# Run Utilities for Better Project Runtime.

# Helps Circumvent CUDA and CUDNN Issues
# during the inference process with the Whisper Model.
# OAPy.forward_CUDA_CUDNN_to_path()

# Initialize the Runtime
OpenAccessibilityPy = OAPy.OpenAccessibilityPy()

## ------------------------------
