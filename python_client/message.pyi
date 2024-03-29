from enum import Enum
from typing import Union


class MessageType(int,Enum): ...

class Message:
    def __init__(self):
        self.m_type: Union[None, MessageType] = ...
        self.message: Union[None, str, bytes] = ...
        ...
    def set_type(self, new_type: MessageType) -> None: ...
    def set_message(self, new_message: Union[str, bytes]) -> None: ...
    def serialize(self) -> bytes: ...
    def decode_text_message(self) -> None: ...
