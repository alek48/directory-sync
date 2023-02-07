import socket
import os
import message as m
import cli_methods as cli
from datetime import datetime


def main():
    """
    """
    sync_dir, sock = initialize_client()
    if sync_dir[-1] != "/":
        sync_dir += "/"
    print("Connected to remote vault. Type 'help' to get a list of available commands. Type 'quit()' to exit")
    while True:
        command = input("->")
        if command == "quit()":
            return
        else:
            msg_out = prepare_command(command)
            send_message(sock, msg_out)
            if "list entries" in command:
                resp = receive_message(sock)
                print(resp.message)
                if resp.message == "OK":
                    receive_dir_message(sock)

            elif "request download" in command:
                command = command.split(" ")
                if len(command) == 3:
                    f_path = sync_dir + command[2]
                    resp = receive_message(sock)
                    print(resp.message)
                    if resp.message == "OK":
                        receive_file(sock, f_path)
                else:
                    resp = receive_message(sock)
                    print(resp.message)

            elif "request upload" in command:
                command = command.split(" ")
                if len(command) == 3:
                    f_path = sync_dir + command[2]
                    resp = receive_message(sock)
                    print(resp.message)
                    if resp.message == "OK":
                        send_file(sock, f_path)
                else:
                    resp = receive_message(sock)
                    print(resp.message)

            else:
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
        data += sock.recv(4-len(data))
    message_size = int.from_bytes(data[:4], "big")
    while len(data) < 8+message_size:
        data += sock.recv(8+message_size-len(data))
    print(data)
    new_message = m.Message()
    type_id = int.from_bytes(data[4:8], "big")
    new_message.set_type(m.MessageType(type_id))
    new_message.set_message(bytes(data[8:]))
    if new_message.m_type == m.MessageType.Text:
        new_message.decode_text_message()
    return new_message


def receive_dir_message(sock):
    """
    Receive DirPart message and print it

    :param sock: socket to receive message from
    """
    data = b''
    received_entries = 0
    while True:
        temp_message = receive_message(sock)
        total_entries = int.from_bytes(temp_message.message[:4], "big")
        received_entries += int.from_bytes(temp_message.message[4:8], "big")
        data += temp_message.message[8:]
        if received_entries == total_entries:
            break
    text = convert_dir_data_to_text(data)
    if text:
        print(text)
    else:
        print("This directory is empty")


def convert_dir_data_to_text(data):
    """
    convert DirPart bytes data into string

    :param data: data of DirPart
    :return: data converted to readable string
    """
    out_text = ""
    split_data = data.split(b'|')
    data_iter = iter(split_data)
    for file, binary_timestamp in zip(data_iter, data_iter):
        out_text += file.decode("UTF-8")
        ts = int.from_bytes(binary_timestamp, "big")
        out_text += str(datetime.fromtimestamp(ts).strftime(' - %Y-%m-%d %H:%M:%S\n'))
    return out_text


def receive_file(sock, f_path):
    """
    Receive and save file from server

    :param sock: socket to receive message from
    :param f_path: global file path on local machine
    """
    os.makedirs(os.path.dirname(f_path), exist_ok=True)
    with open(f_path, 'wb') as f:
        received_data = 0
        while True:
            temp_message = receive_message(sock)
            total_data = int.from_bytes(temp_message.message[:4], "big")
            received_data += int.from_bytes(temp_message.message[4:8], "big")
            f.write(temp_message.message[8:])
            if received_data == total_data:
                break


def send_file(sock, absolute_path):
    """
    sends given file to the server

    :param sock: socket to send
    :param absolute_path: absolute path to file
    """
    if not os.path.isfile(absolute_path):
        print("Can't find the file in the local directory")
        return
    file_size = os.stat(absolute_path).st_size.to_bytes(4, "big")
    with open(absolute_path, 'rb') as f:
        while True:
            data_part = f.read(4096-16)
            if not data_part:
                break
            chunk_size = len(data_part).to_bytes(4, "big")
            chunk_data = file_size+chunk_size+data_part
            msg = m.Message()
            msg.set_type(m.MessageType.FilePart)
            msg.set_message(chunk_data)
            send_message(sock, msg)


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
