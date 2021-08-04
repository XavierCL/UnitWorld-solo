from communications.GameState import GameState
from communications.ServerCommander import ServerCommander

class Artificial:
    def __init__(self, gameController: ServerCommander):
        self.gameController = gameController

    def frame(self, gameState: GameState, currentPlayerId: str):
        raise NotImplementedError
