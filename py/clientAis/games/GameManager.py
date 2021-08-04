from typing import Union

from clientAis.communications.GameState import GameState

class GameManager:
    def __init__(self):
        self.gameState: Union[GameState, None] = None
        self.currentPlayerId: Union[str, None] = None

    def setNextCompleteGameState(self, gameState: GameState):
        self.gameState = gameState

    def setCurrentPlayerId(self, playerId: str):
        self.currentPlayerId = playerId
