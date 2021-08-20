import numpy as np

from clientAis.ais.discreteV1.models.DiscreteGameState import DiscreteGameState
from clientAis.ais.discreteV1.PhysicsEstimator import PhysicsEstimator
from utils import arrays

class DiscreteGameScorer:
    LONG_TERM_ESTIMATION_DURATION = 1e6

    # Returns -1 when all is lost for the current player
    @staticmethod
    def score(gameState: DiscreteGameState, playerId: str) -> float:
        playerIds = list(gameState.playerDictionary.keys())
        playerIndex = arrays.firstIndex(playerIds, playerId)

        if playerIndex == -1:
            return -1

        longTermSinguityCount = [DiscreteGameScorer.getLongTermSinguityCount(gameState, p) for p in playerIds]
        currentPlayerSinguityCount = longTermSinguityCount[playerIndex]
        maxEnemySinguityCount = max(longTermSinguityCount[:playerIndex] + longTermSinguityCount[playerIndex + 1:])

        if currentPlayerSinguityCount == 0:
            return -1
        elif maxEnemySinguityCount == -1:
            return 1
        else:
            return (currentPlayerSinguityCount - maxEnemySinguityCount) / (currentPlayerSinguityCount + maxEnemySinguityCount + 1)

    @staticmethod
    def getLongTermSinguityCount(gameState: DiscreteGameState, playerId: str) -> float:
        ownSpawners = [s for s in gameState.spawners if s.isClaimed() and s.isAllegedToPlayer(playerId)]
        if len(ownSpawners) == 0:
            return 0

        totalGestationFrames = np.sum([s.frameCountBeforeGestationIsDone(gameState.frameCount) for s in ownSpawners])

        return gameState.playerDictionary[playerId].singuityCount\
            + len(ownSpawners) * DiscreteGameScorer.LONG_TERM_ESTIMATION_DURATION * PhysicsEstimator.SPAWNER_SPAWN_PER_FRAME\
            - totalGestationFrames * PhysicsEstimator.SPAWNER_SPAWN_PER_FRAME
