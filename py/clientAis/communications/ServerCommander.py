import time
from typing import List, Set, Union

import numpy as np

from clientAis.communications.JsonHandler import JsonHandler
from clientAis.communications.MessageSerializer import MessageSerializer
from clientAis.networking.CommunicationHandler import CommunicationHandler

class ServerCommander:
    def __init__(self, serverCommunicator: CommunicationHandler, messageSerializer: MessageSerializer):
        self.messageSerializer = messageSerializer
        self.serverCommunicator = serverCommunicator

    def moveUnitsToPosition(self, singuityIds: Union[List[str], Set[str]], position: np.ndarray):
        self.serverCommunicator.send(
            JsonHandler.dump(
                {
                    "type": "move-units-to-position",
                    "data": {"m": singuityIds, "d": {"x": position[0], "y": position[1]}},
                    "timestamp": int(time.time() * 1000)
                }
            ) + "\\n"
        )

    def moveUnitsToSpawner(self, singuityIds: Union[List[str], Set[str]], spawnerId: str):
        self.serverCommunicator.send(
            JsonHandler.dump(
                {
                    "type": "move-units-to-spawner",
                    "data": {"m": singuityIds, "s": spawnerId},
                    "timestamp": int(time.time() * 1000)
                }
            ) + "\\n"
        )

    def setSpawnersRally(self, spawnersId: Union[List[str], Set[str]], mobileDestination: tuple):
        if mobileDestination[0] == "point":
            mobileDestinationJson = {"x": mobileDestination[1][0], "y": mobileDestination[1][1]}
        elif mobileDestination[1] == "allegedSpawner":
            allegenceJson = "n" if mobileDestination[1]["allegence"] is None else {
                "i": mobileDestination[1]["allegence"]["isClaimed"],
                "h": mobileDestination[1]["allegence"]["healthPoints"],
                "p": mobileDestination[1]["allegence"]["playerId"]
            }

            mobileDestinationJson = {"s": {"i": mobileDestination[1]["id"], "a": allegenceJson}}
        else:  # mobileDestination[1] == "spawner"
            mobileDestinationJson = {"u": mobileDestination[1]}

        self.serverCommunicator.send(
            JsonHandler.dump(
                {
                    "type": "set-spawners-rally",
                    "data": {"s": spawnersId, "d": mobileDestinationJson},
                    "timestamp": int(time.time() * 1000)
                }
            ) + "\\n"
        )
