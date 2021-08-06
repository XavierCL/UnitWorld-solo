import sys
import time
from typing import Callable, Dict

from clientAis.ais.Artificial import Artificial
from clientAis.ais.SingleMindClosest.SingleMindClosest import SingleMindClosest
from clientAis.ais.Voider.Voider import Voider
from clientAis.ais.PropagatingVision.PropagatingVision import PropagatingVision
from clientAis.networking.ClientConnector import ClientConnector
from clientAis.communications.MessageSerializer import MessageSerializer
from clientAis.games.GameManager import GameManager
from clientAis.networking.CommunicationHandler import CommunicationHandler
from clientAis.communications.ServerCommander import ServerCommander
from clientAis.communications.ServerReceiver import ServerReceiver
from utils import arrays

commandLineOptions = {key: value for key, value in zip(arrays.soft_accessor(sys.argv, 1, None, 2), arrays.soft_accessor(sys.argv, 2, None, 3))}

DEFAULT_SERVER_IP = "127.0.0.1"
DEFAULT_SERVER_PORT = 52124
DEFAULT_AI_NAME = "propagatingVision"
SECOND_BETWEEN_AI_FRAME = 0.5

serverIp = commandLineOptions.get("serverIp") or DEFAULT_SERVER_IP
serverPort = int(commandLineOptions.get("serverPort") or DEFAULT_SERVER_PORT)
aiName = commandLineOptions.get("aiName") or DEFAULT_AI_NAME

ais: Dict[str, Callable[[ServerCommander], Artificial]] = {
    "voider": lambda serverCommander: Voider(serverCommander),
    "singleMindClosest": lambda serverCommander: SingleMindClosest(serverCommander),
    "propagatingVision": lambda serverCommander: PropagatingVision(serverCommander)
}

def clientConnectorCallBack(communicationHandler: CommunicationHandler):
    print(f"Connected to server at {serverIp}:{serverPort}")

    gameManager = GameManager()

    messageSerializer = MessageSerializer()
    serverCommander = ServerCommander(communicationHandler, messageSerializer)

    artificial = ais[aiName](serverCommander)

    serverReceiver = ServerReceiver(communicationHandler, gameManager, messageSerializer)

    serverReceiver.startAsync()

    print("Starting games evaluation")

    lastAiGameStateVersion = -1

    try:
        while not communicationHandler.closed:
            timeBeforeFrame = time.time()

            if gameManager.gameState is not None and gameManager.gameState.frameCount > lastAiGameStateVersion:
                lastAiGameStateVersion = gameManager.gameState.frameCount
                artificial.frame(gameManager.gameState, gameManager.currentPlayerId)

            timeAfterFrame = time.time()

            frameTimeInSecond = timeAfterFrame - timeBeforeFrame

            if frameTimeInSecond < SECOND_BETWEEN_AI_FRAME:
                print(f"Frame {0 if gameManager.gameState is None else gameManager.gameState.frameCount} took {frameTimeInSecond:.3f}s. Sleeping {SECOND_BETWEEN_AI_FRAME - frameTimeInSecond:.3f}s.")
                time.sleep(SECOND_BETWEEN_AI_FRAME - frameTimeInSecond)
            else:
                print(f"Frame {0 if gameManager.gameState is None else gameManager.gameState.frameCount} took {frameTimeInSecond:.3f}s. Not sleeping")
    except:
        raise
    finally:
        serverReceiver.stop()

ClientConnector(serverIp, serverPort, clientConnectorCallBack)
