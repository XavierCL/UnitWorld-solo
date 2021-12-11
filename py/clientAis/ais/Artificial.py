from clientAis.games.GameState import GameState
from clientAis.communications.ServerCommander import ServerCommander

SECOND_BETWEEN_AI_FRAME = 0.5

class Artificial:
    def __init__(self, serverCommander: ServerCommander):
        self.serverCommander = serverCommander

    def frame(self, gameState: GameState, currentPlayerId: str):
        raise NotImplementedError

    def frameTimeSecond(self) -> float:
        return SECOND_BETWEEN_AI_FRAME
