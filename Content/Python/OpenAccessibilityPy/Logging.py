from enum import Enum


class LogLevel(Enum):
    INFO = 0
    WARNING = 1
    ERROR = 2


def Log(message: str, log_level: LogLevel = LogLevel.INFO):
    """Logs a Message to the Unreal Engine Console.

    Displays the given message in the Unreal Engine Console, with the given log level.
    If the Unreal Engine Python API is not available, the message is printed to the python terminal.

    Args:
        message (str): Message to Log.
        log_level (LogLevel, optional): Log Level of the Message. Defaults to LogLevel.INFO.
    """

    message = f"|| LogOpenAccessibilityPy || {message} ||"

    try:
        from unreal import log, log_warning, log_error

        if log_level == LogLevel.INFO:
            log(message)
        elif log_level == LogLevel.WARNING:
            log_warning(message)
        elif log_level == LogLevel.ERROR:
            log_error(message)
        else:
            log(message)

    except ImportError:
        print(message)
        pass
