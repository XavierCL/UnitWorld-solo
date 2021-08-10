from clientAis.games.GameState import GameState
from clientAis.communications.ServerCommander import ServerCommander

class Artificial:
    def __init__(self, serverCommander: ServerCommander):
        self.serverCommander = serverCommander

    def frame(self, gameState: GameState, currentPlayerId: str):
        raise NotImplementedError
