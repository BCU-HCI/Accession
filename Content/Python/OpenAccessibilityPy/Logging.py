from enum import Enum
import unreal as ue


class LogLevel(Enum):
    INFO = 0
    WARNING = 1
    ERROR = 2


def Log(message: str, log_level: LogLevel = LogLevel.INFO):

    message = f"|| LogOpenAccessibilityPy || {message} ||"

    if log_level == LogLevel.INFO:
        ue.log(message)
    elif log_level == LogLevel.WARNING:
        ue.log_warning(message)
    elif log_level == LogLevel.ERROR:
        ue.log_error(message)
    else:
        ue.log(message)
