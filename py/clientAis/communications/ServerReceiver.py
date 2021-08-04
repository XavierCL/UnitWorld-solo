import threading

from clientAis.communications.GameState import GameStateMessage
from clientAis.communications.MessageSerializer import MessageSerializer
from clientAis.games.GameManager import GameManager
from clientAis.networking.CommunicationHandler import CommunicationHandler

class ServerReceiver:
    def __init__(self, communicationHandler: CommunicationHandler, gameManager: GameManager, messageSerializer: MessageSerializer):
        self.communicationHandler = communicationHandler
        self.gameManager = gameManager
        self.messageSerializer = messageSerializer

        self.loopThread = None
        self.lastReceivedTimestamp = 0

    def startAsync(self):
        self.loopThread = threading.Thread(target=ServerReceiver.loopReceive, args=(self,))
        self.loopThread.start()

    def stop(self):
        self.communicationHandler.close()
        self.loopThread.join()

    def loopReceive(self):
        while not self.communicationHandler.closed:
            communication = self.communicationHandler.receive()
            communications = self.messageSerializer.deserialize(communication)

            for communication in communications:
                if communication["timestamp"] > self.lastReceivedTimestamp:
                    self.lastReceivedTimestamp = communication["timestamp"]
                    gameStateMessage = GameStateMessage(communication["data"])
                    self.gameManager.setCurrentPlayerId(gameStateMessage.currentPlayer)
                    self.gameManager.setNextCompleteGameState(gameStateMessage.gameState)