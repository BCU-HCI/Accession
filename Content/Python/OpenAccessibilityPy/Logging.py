from enum import Enum


class LogLevel(Enum):
    INFO = 0
    WARNING = 1
    ERROR = 2


def Log(message: str, log_level: LogLevel = LogLevel.INFO):

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
