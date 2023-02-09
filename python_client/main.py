import socket
import os
import message as m
import cli_methods as cli
from datetime import datetime
from time import sleep, time


def main():
    sync_dir, sock = initialize_client()
    if sync_dir[-1] != "/":
        sync_dir += "/"
    print("Connected to remote vault. Type 'help' to get a list of available commands. Type 'quit()' to exit")
    status = "connected"
    try:
        while True:
            command = input("->")
            if command == "quit()":
                sock.close()
                return
            elif command == "auto()":
                if status == "synced":
                    print("Not implemented")
                else:
                    print("Not synced to a vault, cannot auto()")
            else:
                msg_out = prepare_command(command)
                send_message(sock, msg_out)
                if "list entries" in command:
                    resp = receive_message(sock)
                    if resp.message == "OK":
                        receive_dir_message(sock)

                elif "request download" in command:
                    command = command.split(" ")
                    if len(command) == 3:
                        f_path = sync_dir + command[2]
                        resp = receive_message(sock)
                        if resp.message == "OK":
                            receive_file(sock, f_path)
                        else:
                            resp = receive_message(sock)
                            print(resp.message)
                    else:
                        resp = receive_message(sock)
                        print(resp.message)

                elif "request upload" in command:
                    command = command.split(" ")
                    if len(command) == 3:
                        f_path = sync_dir + command[2]
                        if not os.path.isfile(f_path):
                            print("This file does not exist locally")
                            continue
                        resp = receive_message(sock)
                        if resp.message == "OK":
                            send_file(sock, f_path)
                        else:
                            resp = receive_message(sock)
                            print(resp.message)
                    else:
                        resp = receive_message(sock)
                        print(resp.message)

                else:
                    resp = receive_message(sock)
                    if resp.message == "Syncing":
                        status = "synced"
                    print(resp.message)
                    print("Type 'auto()' to start automatic syncing")
    except socket.timeout or ConnectionAbortedError:
        print("Server is not responding or has shut down, exiting...")


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
    s.settimeout(10)
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
    new_message = m.Message()
    type_id = int.from_bytes(data[4:8], "big")
    new_message.set_type(m.MessageType(type_id))
    new_message.set_message(bytes(data[8:]))
    if new_message.m_type == m.MessageType.Text:
        new_message.decode_text_message()
    return new_message


def receive_dir_message(sock, to_cli=True):
    """
    Receive DirPart message and print it

    :param sock: socket to receive message from
    :param to_cli: should the message be printed out in cli
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
    if to_cli:
        text = convert_dir_data_to_text(data)
        if text:
            print(text)
        else:
            print("This directory is empty")
    else:
        return convert_dir_data_to_text(data, False)


def convert_dir_data_to_text(data, readable_date=True):
    """
    convert DirPart bytes data into string

    :param data: data of DirPart
    :param readable_date: should the timestamp be converted to readable date
    :return: data converted to readable string
    """
    out_text = ""
    split_data = data.split(b'|')
    data_iter = iter(split_data)
    for file, binary_timestamp in zip(data_iter, data_iter):
        out_text += file.decode("UTF-8")
        ts = int.from_bytes(binary_timestamp, "big")
        if readable_date:
            out_text += str(datetime.fromtimestamp(ts).strftime(' - %Y-%m-%d %H:%M:%S\n'))
        else:
            out_text += "|"+str(ts)+"\n"
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
            chunk_size = len(data_part).to_bytes(4, "big")
            chunk_data = file_size+chunk_size+data_part
            msg = m.Message()
            msg.set_type(m.MessageType.FilePart)
            msg.set_message(chunk_data)
            send_message(sock, msg)
            if not data_part:
                break
    os.utime(absolute_path, (time(), time()))


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


def autosync_loop(sock, dir_path):
    """
    Automatically syncs local and remote directory

    :param sock: socket with server connection
    :param dir_path: path to local directory
    """
    while True:
        sleep(5)
        local_files = find_all_local_files(dir_path)
        msg_out = prepare_command("list entries")
        send_message(sock, msg_out)
        resp = receive_message(sock)
        if resp.message == "OK":
            remote_files = receive_dir_message(sock, to_cli=False).strip()
            remote_files = [x.split("|") for x in remote_files.split("\n")]
            remote_files = [[x[0], float(x[1])] for x in remote_files]
        else:
            continue
        local_files = iter(local_files)
        remote_files = iter(remote_files)
        files_to_push = []
        files_to_pull = []
        try:
            f_local = next(local_files)
            f_remote = next(remote_files)
            while True:
                if f_local[0] == f_remote[0]:
                    if f_local[1] - f_remote[1] > 1:
                        files_to_push.append(f_local[0])
                    elif f_local[1] - f_remote[1] < 1:
                        files_to_pull.append(f_remote[0])
                    f_remote = next(remote_files)
                    f_local = next(local_files)
                elif f_local[0] < f_remote[0]:
                    files_to_push.append(f_local[0])
                    f_local = next(local_files)
                else:
                    while f_local[0] > f_remote[0]:
                        files_to_pull.append(f_remote[0])
                        f_remote = next(remote_files)
        except StopIteration:
            pass
        for f in local_files:
            files_to_push.append(f)
        for f in remote_files:
            files_to_pull.append(f)

        for file in files_to_push:
            msg_out = prepare_command("request upload "+file)
            send_message(sock, msg_out)
            resp = receive_message(sock)
            if resp.message == "OK":
                send_file(sock, dir_path+file)
            else:
                pass
            sleep(1)

        for file in files_to_pull:
            msg_out = prepare_command("request download "+file)
            send_message(sock, msg_out)
            resp = receive_message(sock)
            if resp.message == "OK":
                receive_file(sock, dir_path+file)
            else:
                pass
            sleep(1)


def find_all_local_files(directory_path, __recursive_path=""):
    """
    Given a directory path, returns a list of all files in that directory and sub-directories

    :param directory_path: path to the directory
    :param __recursive_path: used in recursive calls, do not change
    :return: list of pairs [relative path, last file edit time]
    """
    list_of_files = []
    directory_contents = os.listdir(path=directory_path+__recursive_path)
    for entry in directory_contents:
        full_path = directory_path+__recursive_path+entry
        if os.path.isfile(full_path):
            list_of_files.append((__recursive_path+entry, os.path.getmtime(full_path)))
        if os.path.isdir(full_path):
            list_of_files += find_all_local_files(directory_path, __recursive_path+entry+"/")
    return list_of_files


if __name__ == '__main__':
    main()
