import socket
import message as m
"""
========
small mock server for testing purposes
========
"""
HOST = "127.0.0.1"
PORT = 1515
print("starting...")
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    print(s.bind((HOST, PORT)))
    s.listen()
    conn, addr = s.accept()
    with conn:
        print(f"Connected by {addr}")
        msg = m.Message()
        msg.set_message("test text messageąęó hehe")
        msg.set_type(m.MessageType.Text)
        data = msg.serialize()
        conn.sendall(data)
