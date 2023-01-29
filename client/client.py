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

# data should be max 4096 bytes
def createMessage(type, data):
    return intToBytes(len(data)) + intToBytes(type) + data

def readMessage(data):
    len = bytesToInt(data[0:4])
    type = bytesToInt(data[4:8])
    messageData = data[8:]
    if (type == MESSAGE_TYPES['Text']):
        print(bytesToString(messageData))


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.sendall(createMessage(MESSAGE_TYPES['Text'], b'Hello'))
    s.sendall(createMessage(MESSAGE_TYPES['Text'], b'ab'))
    s.sendall(createMessage(MESSAGE_TYPES['Text'], b'c'))
    s.sendall(createMessage(MESSAGE_TYPES['Text'], b'd'))
    s.sendall(createMessage(MESSAGE_TYPES['Command'], b'list'))
    data = s.recv(4096)
    readMessage(data)

#print(f"Received {data!r}")