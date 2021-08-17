import math

import numpy as np

from clientAis.ais.discreteV1.models.DiscreteGameState import DiscreteGameState
from clientAis.games.GameState import Spawner

class DiscreteGameScorer:

    # Returns -1 when all is lost for the current player
    @staticmethod
    def score(gameState: DiscreteGameState, playerIndex: int) -> float:
        return DiscreteGameScorer.ownSpawnerRatio(gameState, gameState.playerIds[playerIndex])

    @staticmethod
    def ownSpawnerRatio(gameState: DiscreteGameState, playerId: str) -> float:
        # Should take singuities strength and spawner gestation in account
        ownPlayer = gameState.playerDictionary[playerId]
        ownSpawners = [s for s in gameState.spawners if s.isClaimed() and s.isAllegedToPlayer(playerId)]
        baseScoreOverSpawnerCount = len(ownSpawners)

        ownAllegedSpawners = [s for s in gameState.spawners if s.isAllegedToPlayer(playerId)]

        ownSpawnerHealths = np.array([s.getHealthPoints() for s in ownAllegedSpawners])
        ownSpawnerHealthsOverThird = ownSpawnerHealths / (Spawner.MAX_HEALTH_POINTS * 3)
        ownSpawnerGestationRatioOverThird = (Spawner.GESTATION_FRAME_LAG - np.array([s.frameCountBeforeGestationIsDone(gameState.frameCount) for s in ownAllegedSpawners])) / (Spawner.GESTATION_FRAME_LAG * 3)
        singuityStrength = 1 / (ownPlayer.singuityCount / (1 + ownPlayer.singuitiesStd) + 1)
        biasedOwnSpawnerHealths = np.sqrt(ownSpawnerHealthsOverThird + ownSpawnerGestationRatioOverThird)
        ownSpawnerHealthOver1 = np.sum(biasedOwnSpawnerHealths) / len(gameState.spawners) + singuityStrength * (1 - math.sqrt(2/3))

        return 2 * (baseScoreOverSpawnerCount + ownSpawnerHealthOver1) / len(gameState.spawners) - 1
