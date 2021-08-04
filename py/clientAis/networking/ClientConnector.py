import socket
from typing import Any, Callable

from clientAis.networking.CommunicationHandler import CommunicationHandler

class ClientConnector:
    def __init__(self, serverIp: str, serverPort: int, clientConnectedCallback: Callable[[CommunicationHandler], Any]):
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        endpoint = (serverIp, serverPort)
        sock.connect(endpoint)
        clientConnectedCallback(CommunicationHandler(sock, endpoint))
