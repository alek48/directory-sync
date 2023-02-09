import socket
from _typeshed import AnyPath
import message
from typing import Union,List


def main() -> None: ...
def initialize_client() -> tuple[AnyPath, socket.socket()]: ...
def send_message(sock: socket.socket, msg: message.Message()) -> None: ...
def receive_message(sock: socket.socket) -> message.Message(): ...
def prepare_command(comm: str) -> message.Message(): ...
def receive_dir_message(sock: socket.socket, to_cli: bool = True) -> Union[None,str]: ...
def convert_dir_data_to_text(data: bytes,readable_date: bool = True) -> str: ...
def receive_file(sock: socket.socket, f_path: AnyPath) -> None: ...
def send_file(sock: socket , absolute_path: AnyPath) -> None: ...
def autosync_loop(sock: socket, dir_path: AnyPath) -> None: ...
def find_all_local_files(directory_path: AnyPath, __recursive_path: str="") -> List[(str, float)]: ...