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

    @staticmethod
    def frameIsIgnored(gameState: GameState, currentPlayerId: str) -> bool:
        if len(gameState.players) <= 1:
            # Waiting for other players
            return True

        notOwnSpawners = [s for s in gameState.spawners if s.allegence is None or not s.allegence.isClaimed or (s.allegence.isClaimed and s.allegence.playerId != currentPlayerId)]

        if len(notOwnSpawners) == 0:
            # Game is won
            return True

        ownSpawners = [s for s in gameState.spawners if s.allegence is not None and s.allegence.isClaimed and s.allegence.playerId == currentPlayerId]

        if len(ownSpawners) == 0:
            # Game is lost
            return True

        ownSinguities = [s for s in gameState.singuities if s.playerId == currentPlayerId]

        if len(ownSinguities) == 0:
            # No units to control
            return True

        return False