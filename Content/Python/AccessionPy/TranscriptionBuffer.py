import unreal as ue
import queue
from dataclasses import dataclass
from collections import OrderedDict
from threading import Lock


@dataclass
class TranscriptionState:
    id: ue.Guid
    segments: ue.Array[str]


class TranscriptionBuffer:

    def __init__(self):

        self.store: OrderedDict[ue.Guid, TranscriptionState] = OrderedDict()
        self.actives: set[ue.Guid] = set()

        self._lock = Lock()

    def register_active(self, id: ue.Guid):
        with self._lock:
            self.actives.add(id)

    def unregister_active(self, id: ue.Guid):
        with self._lock:
            self.actives.discard(id)

    def store_completed(self, id: ue.Guid, segments: list[str]):
        with self._lock:
            self.store[id] = TranscriptionState(id, segments)

    def remove_completed(self, id: ue.Guid):
        with self._lock:
            try:
                self.store.pop(id)
            except KeyError:
                pass

    def process_completed(self, processed_callback):

        with self._lock:

            while len(self.store) > 0:

                id, state = self.store.popitem(last=False)

                self.actives.discard(id)

                processed_callback(id, state.segments)
