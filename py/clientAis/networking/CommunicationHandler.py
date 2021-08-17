import socket
from typing import Tuple

class CommunicationHandler:
    def __init__(self, sock: socket.socket, endpoint: Tuple[str, int]):
        self.sock = sock
        self.endpoint = endpoint
        self.closed = False
        self.BUFFER_SIZE = 65535

    def send(self, communication: str):
        if self.closed:
            raise Exception("Cannot send data through a disconnected socket.")

        try:
            return self.sock.send(communication.encode("ascii"))
        except:
            self.closed = True

    def receive(self) -> str:
        if self.closed:
            raise Exception("Cannot receive data through a disconnected socket.")

        try:
            communication = self.sock.recv(self.BUFFER_SIZE).decode("ascii")

            if len(communication) == 0:
                self.closed = True
                raise Exception("Remote socket has been shutdown while receiving from it")

            return communication

        except:
            self.closed = True
            raise

    def close(self):
        if self.closed:
            return

        self.closed = True
        return self.sock.close()
