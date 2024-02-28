import unreal

import subprocess
import pkg_resources

from os import getcwd

unreal.log(f"|| Saved Location || {unreal.Paths.project_saved_dir()}")

unreal.log("|| OpenAccessibility Python || Initializing")

# Dependencies of the Project
dependencies = {"faster-whisper", "pyzmq"}
installed = {pkg.key for pkg in pkg_resources.working_set}

missing_dependencies = dependencies - installed

if missing_dependencies:
    unreal.log_warning(
        "|| OpenAccessibility Python || Missing Dependencies Detected ||"
    )

    with unreal.ScopedSlowTask(
        len(missing_dependencies), "OpenAccessibilty Installing Python Dependencies"
    ) as slow_task:

        # Create a Dialog for UI Feedback
        slow_task.make_dialog(True)

        for depNum, depName in enumerate(missing_dependencies):
            slow_task.enter_progress_frame(
                1,
                f"Installing Dependency {depNum} / {missing_dependencies.__len__()}: {depName}",
            )
            unreal.log_warning(
                f"|| OpenAccessibility Python || Installing {depName} ||"
            )

            subprocess.check_call(
                [
                    unreal.get_interpreter_executable_path(),
                    "-m",
                    "pip",
                    "install",
                    depName,
                ],
            )
else:
    unreal.log(
        "|| OpenAccessibility Python || All Dependencies are already installed ||"
    )

import OpenAccessibilityPy as OAPy

unreal.log("|| OpenAccessibility Python || Initializing Python Runtime ||")

# Initialize the Python Runtime
OpenAccessibilityPy = OAPy.OpenAccessibilityPy()


def OnPythonShutdown():
    unreal.log("|| OpenAccessibility Python || Python Runtime Shutdown ||")
    del OpenAccessibilityPy


unreal.register_python_shutdown_callback(OnPythonShutdown)
