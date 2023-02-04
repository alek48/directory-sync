import socket
import os
import message as m
import cli_methods as cli
import enum


def main():
    """
    :return: None
    """
    sync_dir, sock = initialize_client()
    print("Connected to remote vault. Type 'help' to get a list of available commands. Type 'quit()' to exit")
    while True:
        command = input("->")
        if command == "quit()":
            return
        else:
            msg_out = prepare_command(command)
            send_message(sock, msg_out)
            resp = receive_message(sock)
            print(resp.message)


def initialize_client():
    """
    Prepares socket and directory for the client
    :return: (directory path, socket handle)
    """
    print("=========\n"
          "Initializing client...")
    directory = cli.get_sync_directory()
    print("Directory set, connecting to server...")
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    while True:
        ip_addr, port = cli.get_server_ip()
        try:
            s.connect((ip_addr, port))
        except Exception as e:
            print("Connection failed")
            print(e)
        else:
            break
    print("Initialization complete.\n"
          "==========")
    return directory, s


def find_files_to_update(directory_path, compared_timestamp):
    """
    Recursively searches the directory and returns list of all files edited more recently than given timestamp

    :param directory_path: directory to search
    :param compared_timestamp: timestamp to compare
    :return: list of files
    """
    list_of_files = []
    directory_contents = os.listdir(path=directory_path)
    for entry in directory_contents:
        full_path = directory_path+entry
        if os.path.getmtime(full_path) > compared_timestamp:
            if os.path.isfile(full_path):
                list_of_files.append(full_path)
            if os.path.isdir(full_path):
                list_of_files += find_files_to_update(full_path, compared_timestamp)
    return list_of_files


def send_message(sock, msg):
    """
    Sends a message object on given socket
    :param sock: socket object
    :param msg: message object
    :return: None
    """
    sock.sendall(msg.serialize())


def receive_message(sock):
    """
    Listens on given socket for data and formats it into a message object

    :param sock: socket to receive message from
    :return: Message object filled with received data
    """
    data = b''
    while len(data) < 4:
        data += sock.recv(4+4+4096)
    while len(data) < int.from_bytes(data[:4], "big"):
        data += sock.recv(4+4+4096)
    new_message = m.Message()
    new_message.set_type(enum.Enum(m.MessageType, int.from_bytes(data[4:8], "big")))
    new_message.set_message(bytes(data[8:]))
    if new_message.m_type in [m.MessageType.Text, m.MessageType.DirPart]:
        new_message = decode_text_message(new_message)
    return new_message


def decode_text_message(msg):
    """
    Decodes byte message into text string

    :param msg: message object to decode into text
    :return: message object with decoded data
    """
    msg.set_message(msg.message.decode("UTF-8"))
    return msg


def prepare_command(comm):
    """
    creates message object with given command
    :param comm: command to use in object
    :return: Message object
    """
    msg_out = m.Message()
    msg_out.set_type(m.MessageType.Command)
    msg_out.set_message(comm.strip())
    return msg_out


if __name__ == '__main__':
    main()
