import unreal

import subprocess
import pkg_resources

unreal.log("|| OpenAccessibility Python || Initializing")

# Dependencies of the Project
DEPS = ["faster-whisper", "pyzmq", "av"]
installed = {pkg for pkg in pkg_resources.working_set}

missing_dependencies = DEPS - installed

if missing_dependencies:
    unreal.log_warning(
        "|| OpenAccessibility Python || Missing Dependencies Detected ||"
    )

    with unreal.ScopedSlowTask(
        len(missing_dependencies), "OpenAccessibilty Installing Python Dependencies"
    ) as slow_task:

        # Create a Dialog for UI Feedback
        slow_task.make_dialog(can_cancel=False)

        for depNum, depName in enumerate(missing_dependencies):
            unreal.log_warning(
                f"|| OpenAccessibility Python || Installing {depName} ||"
            )

            slow_task.enter_progress_frame(
                0.5,
                f"Installing Dependency {depNum} / {len(missing_dependencies)}: {depName}",
            )

            process = subprocess.Popen(
                [
                    unreal.get_interpreter_executable_path(),
                    "-m",
                    "pip",
                    "install",
                    depName,
                ],
                shell=True,
            )

            while process.poll() is None:

                slow_task.enter_progress_frame(
                    0.5,
                    f"Installed Dependency {depNum} / {len(missing_dependencies)}: {depName}",
                )
else:
    unreal.log(
        "|| OpenAccessibility Python || All Dependencies are already installed ||"
    )

import OpenAccessibilityPy as OAPy

unreal.log("|| OpenAccessibility Python || Initializing Python Runtime ||")

# Run Utilities for Better Project Library Initialization

# Helps Circumvent CUDA and CUDNN Issues
# when using the Whisper Model
OAPy.forward_CUDA_CUDNN_to_path()

# Initialize the Python Runtime
OpenAccessibilityPy = OAPy.OpenAccessibilityPy()
