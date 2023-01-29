import socket

HOST = "localhost"  # The server's hostname or IP address
PORT = 1515  # The port used by the server

MESSAGE_TYPES = {
    'Text': 0,
    'Command': 1,
    'FilePart': 2,
    'DirPart': 3
}

def intToBytes(val):
    return val.to_bytes(4, 'big') # 4 bytes in big endian

def bytesToInt(b_val):
    return int.from_bytes(b_val, "big")

def stringToBytes(val):
    return val.encode()

def bytesToString(b_val):
    return b_val.decode()  

class Message:
    def __init__(self, m_len, m_type, m_data):
        self.len = m_len
        self.type = m_type
        self.data = m_data

    def getTypeMessage(self):
        if self.type == MESSAGE_TYPES['Text']:
            return TextMessage(bytesToString(self.data))
        elif self.type == MESSAGE_TYPES['Command']:
            return CommandMessage(bytesToString(self.data))
        elif self.type == MESSAGE_TYPES['FilePart']:
            pass
        elif self.type == MESSAGE_TYPES['DirPart']:
            pass

class TextMessage:
    def __init__(self, m_text):
        self.text = m_text

    def createMessage(self):
        b_data = stringToBytes(self.text)
        return Message(len(b_data), MESSAGE_TYPES['Text'], b_data)

class CommandMessage:
    def __init__(self, m_text):
        self.parts = m_text

    def createMessage(self):
        b_data = stringToBytes(self.parts)
        return Message(len(b_data), MESSAGE_TYPES['Command'], b_data)

# data should be max 4096 bytes
def createMessage(type, data):
    return intToBytes(len(data)) + intToBytes(type) + data

def serializeMessage(message):
    return intToBytes(len(message.data)) + intToBytes(message.type) + message.data

def readMessage(data):
    len = bytesToInt(data[0:4])
    type = bytesToInt(data[4:8])
    messageData = data[8:]
    return Message(len, type, messageData)


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))

    message = Message(0, 0, 0)
    typedMessage = TextMessage("")
    syncing = False
    
    while not syncing:
        user_input = input()
        command = CommandMessage(user_input)

        s.sendall(serializeMessage(command.createMessage()))

        data = s.recv(4096+4+4)
        message = readMessage(data)
        typedMessage = message.getTypeMessage()

        if (not isinstance(typedMessage, TextMessage)):
            print("Error: not a text message")
        
        if (typedMessage.text == "Syncing"):
            syncing = False ###### False for testing ######

        print(typedMessage.text)

