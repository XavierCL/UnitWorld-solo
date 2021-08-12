import numpy as np

from clientAis.ais.discreteV1.DiscreteGameState import DiscreteGameState
from clientAis.games.GameState import Spawner

class DiscreteGameScorer:
    # Returns -1 when all is lost for the current player
    def score(self, gameState: DiscreteGameState, playerIndex: int) -> float:
        return self.ownSpawnerRatio(gameState, gameState.playerIds[playerIndex])

    def ownSpawnerRatio(self, gameState: DiscreteGameState, playerId: str) -> float:
        ownSpawners = [s for s in gameState.spawners if s.isClaimed() and s.isAllegedToPlayer(playerId)]
        return 2 * (len(ownSpawners) + np.sum([s.getHealthPoints() for s in gameState.spawners if s.isAllegedToPlayer(playerId)]) / (Spawner.MAX_HEALTH_POINTS * len(gameState.spawners))) / (len(gameState.spawners) + 1) - 1
