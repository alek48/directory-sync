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

def stringToBytes(val):
    return val.encode()  

# data should be max 4096 bytes
def createMessage(type, data):
    return intToBytes(len(data)) + intToBytes(type) + data

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.sendall(createMessage(MESSAGE_TYPES['Text'], b'Hello'))
    s.sendall(createMessage(MESSAGE_TYPES['Command'], b'Hello World'))
    #data = s.recv(1024)

#print(f"Received {data!r}")