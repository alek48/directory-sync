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
            numOfAllEntries = bytesToInt(self.data[0:4])
            numOfEntries = bytesToInt(self.data[4:8])
            entries = []
            dataIdx = 8
            for i in range(numOfEntries):
                filePath = ''
                modDate = ''

                while dataIdx < len(data) and bytesToString(self.data[dataIdx:dataIdx+1]) != '|':
                    filePath += bytesToString(self.data[dataIdx:dataIdx+1])
                    dataIdx += 1
                dataIdx += 1

                modDate = bytesToInt(self.data[dataIdx:dataIdx+4])
                dataIdx += 5

                entries.append(DirEntry(filePath, modDate))

            return DirPartMessage(numOfAllEntries, numOfEntries, entries)


class TextMessage:
    def __init__(self, m_text):
        self.text = m_text

    def createMessage(self):
        b_data = stringToBytes(self.text)
        return Message(len(b_data), MESSAGE_TYPES['Text'], b_data)

    def print(self):
        print(self.text)

class CommandMessage:
    def __init__(self, m_text):
        self.parts = m_text

    def createMessage(self):
        b_data = stringToBytes(self.parts)
        return Message(len(b_data), MESSAGE_TYPES['Command'], b_data)
    
    def print(self):
        print(self.parts)

class DirPartMessage:
    def __init__(self, numOfAllEntries, numOfEntries, entries):
        self.numOfAllEntries = numOfAllEntries
        self.numOfEntries = numOfEntries
        self.entries = entries

    def print(self):
        for entry in self.entries:
            entry.print()

class DirEntry:
    def __init__(self, filePath, modDate):
        self.filePath = filePath
        self.modDate = modDate
    
    def print(self):
        print(self.filePath, ';', self.modDate)

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


def downloadFile():
    pass

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))

    message = Message(0, 0, 0)
    typedMessage = TextMessage("")
    syncing = False
    
    while not syncing:
        user_input = "sync 123 force" # input()
        command = CommandMessage(user_input)

        s.sendall(serializeMessage(command.createMessage()))
        print("sent")

        data = s.recv(4096+4+4)
        message = readMessage(data)
        typedMessage = message.getTypeMessage()

        if (not isinstance(typedMessage, TextMessage)):
            print("Error: not a text message")
        
        if (typedMessage.text == "Syncing"):
            syncing = True

        typedMessage.print()

    while syncing:
        user_input = "list entries"
        command = CommandMessage(user_input)

        s.sendall(serializeMessage(command.createMessage()))
        print("sent")

        data = s.recv(4096+4+4)

        message = readMessage(data) # OK

        data = s.recv(4096+4+4)
        message = readMessage(data)
        entries = message.getTypeMessage()
        
        # TODO
        # compare entries with local entries;
        # if sync force was successful then client can override remote files

        # download:
        # file not present in local storage => request download
        # file present in local storage but older => request download or skip (for client to determine)
        # upload:
        # file not present in remote storage => request upload
        # file present in remote storage but older => request upload (only when sync force)

        for entry in entries.entries:
            command = CommandMessage("request download " + entry.filePath)
            s.sendall(serializeMessage(command.createMessage()))

            data = s.recv(4096+4+4)
            message = readMessage(data)
            textMessage = message.getTypeMessage()
            if textMessage.text == "OK":
                downloadFile()

        # command = CommandMessage("request upload " + "b/file.txt")
        # s.sendall(serializeMessage(command.createMessage()))
        # data = s.recv(4096+4+4)

        # file_data = "asdsbbbbbbbbbbbbbbbbeeeeeeeeeeasdf"
        # d = intToBytes(len(file_data)) + intToBytes(len(file_data)) + stringToBytes(file_data)
        # message = Message(len(d), MESSAGE_TYPES['FilePart'], d)
        # s.sendall(serializeMessage(message))


