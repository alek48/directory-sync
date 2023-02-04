from enum import Enum
import struct


class MessageType(int, Enum):
    Text = 0
    Command = 1
    FilePart = 2
    DirPart = 3


class Message:
    def __init__(self):
        self.m_type = None
        self.message = None

    def set_type(self, new_type):
        """
        Sets the message type
        :param new_type: type of message
        """
        self.m_type = new_type

    def set_message(self, new_message):
        """
        Sets the message content
        :param new_message:
        """
        self.message = new_message

    def serialize(self):
        """
        Returns the object converted to bytes, compatible with C++ struct used on the server

        The order is: (length of message data, type of message, message content)
        Uses big endian

        :return: byte representation of object
        """
        if type(self.message) == str:
            encoded_message = self.message.encode()
        elif type(self.message) == bytes:
            encoded_message = self.message
        else:
            print("Unexpected message type" + str(type(self.message)))
            return struct.pack(f">ii", 0, self.m_type)  # TODO: better error handling
        m_len = len(encoded_message)
        return struct.pack(f">ii{m_len}s", m_len, self.m_type, encoded_message)
