import sys
import time
from typing import Callable, Dict

from clientAis.ais.Artificial import Artificial
from clientAis.ais.discreteV1.DiscreteV1Ai import DiscreteV1Ai
from clientAis.ais.packing.PackingAi import PackingAi
from clientAis.ais.packingAhead.PackingAheadAi import PackingAheadAi
from clientAis.ais.packingBehind.PackingBehindAi import PackingBehindAi
from clientAis.ais.quickAttack.QuickAttackAi import QuickAttackAi
from clientAis.ais.singleMindClosest.SingleMindClosestAi import SingleMindClosestAi
from clientAis.ais.Voider.VoiderAi import VoiderAi
from clientAis.ais.propagatingVision.PropagatingVisionAi import PropagatingVisionAi
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
DEFAULT_AI_NAME = "discreteV1"

serverIp = commandLineOptions.get("serverIp") or DEFAULT_SERVER_IP
serverPort = int(commandLineOptions.get("serverPort") or DEFAULT_SERVER_PORT)
aiName = commandLineOptions.get("aiName") or DEFAULT_AI_NAME

ais: Dict[str, Callable[[ServerCommander], Artificial]] = {
    "voider": lambda serverCommander: VoiderAi(serverCommander),
    "singleMindClosest": lambda serverCommander: SingleMindClosestAi(serverCommander),
    "propagatingVision": lambda serverCommander: PropagatingVisionAi(serverCommander),
    "packing": lambda serverCommander: PackingAi(serverCommander),
    "quickAttack": lambda serverCommander: QuickAttackAi(serverCommander),
    "packingAhead": lambda serverCommander: PackingAheadAi(serverCommander),
    "packingBehind": lambda serverCommander: PackingBehindAi(serverCommander),
    "discreteV1": lambda serverCommander: DiscreteV1Ai(serverCommander)
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

            hasNewFrame = gameManager.gameState is not None and gameManager.gameState.frameCount > lastAiGameStateVersion

            if hasNewFrame:
                lastAiGameStateVersion = gameManager.gameState.frameCount
                artificial.frame(gameManager.gameState, gameManager.currentPlayerId)

            timeAfterFrame = time.time()

            frameTimeInSecond = timeAfterFrame - timeBeforeFrame

            allottedFrameTime = artificial.frameTimeSecond()

            if not hasNewFrame:
                print(f"No new frame. Sleeping {allottedFrameTime:.3f}s.")
                time.sleep(allottedFrameTime)
            elif frameTimeInSecond < allottedFrameTime:
                print(f"Frame {0 if gameManager.gameState is None else gameManager.gameState.frameCount} took {frameTimeInSecond:.3f}s. Sleeping {allottedFrameTime - frameTimeInSecond:.3f}s.")
                time.sleep(allottedFrameTime - frameTimeInSecond)
            else:
                print(f"Frame {0 if gameManager.gameState is None else gameManager.gameState.frameCount} took {frameTimeInSecond:.3f}s. Not sleeping")
                time.sleep(0.001)
    except:
        raise
    finally:
        serverReceiver.stop()

ClientConnector(serverIp, serverPort, clientConnectorCallBack)
