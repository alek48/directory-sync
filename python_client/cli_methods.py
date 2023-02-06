from os import path, listdir
import re


def get_sync_directory():
    """
    Asks user for directory path, validates it, and returs it
    :return: directory path
    """
    while True:
        dir_path = input("Please enter path to the directory you want to share:\n"
                         "->")
        if validate_directory(dir_path):
            return dir_path


def validate_directory(directory):
    """
    Checks if directory exists and asks user for confirmation if directory is not empty
    :param directory: path to directory
    :return: is validated? bool
    """
    if path.isdir(directory):
        if listdir(directory):
            if input("This directory already contains some files, "
                     "this data might be lost. Do you want to continue? [y/n]\n"
                     "->") == "y":
                return True
            else:
                return False
        return True
    return False


def get_server_ip():
    """
    Asks user for IPv4 and port of the target server, then validates them
    :return: tuple (IPv4, port)
    """
    while True:
        ip_addr = input("Please provide the address of syncing\n"
                        "->").strip()
        if validate_ip(ip_addr):
            break
        print("Incorrect IP address, try again")
    while True:
        port = (input("Please provide the port of syncing server\n"
                      "->"))
        if validate_port(port):
            break
        print("Incorrect port number, try again")
    return ip_addr, int(port)


def validate_ip(addr):
    """
    Checks if given string is a valid IPv4 address
    :param addr: string to test
    :return: Is valid? bool
    """
    regex = re.search(r"^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}$", addr)
    if (regex is None) or (len(regex.group()) != len(addr)):
        return False
    for elem in addr.split("."):
        part = int(elem)
        print(part)
        if not 0 <= part <= 255:
            return False
    return True


def validate_port(port):
    """
    Checks if given string is a valid port number
    :param port: string to check
    :return: Is valid? bool
    """
    try:
        port = int(port)
    except ValueError:
        return False
    if (0 > port >= 65535):
        return False
    return True
