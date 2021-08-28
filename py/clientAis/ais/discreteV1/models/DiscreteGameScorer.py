import numpy as np

from clientAis.ais.discreteV1.models.DiscreteGameState import DiscreteGameState
from clientAis.ais.discreteV1.physics.PhysicsEstimator import PhysicsEstimator
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

        if len(gameState.playerDictionary) < 2 and playerId in gameState.playerDictionary:
            return 1
        elif len(gameState.playerDictionary) < 2:
            return -1

        quickAttackDuration = [DiscreteGameScorer.quickAttackDuration(gameState, p) for p in playerIds]
        currentPlayerSinguityCount = quickAttackDuration[playerIndex]
        maxEnemySinguityCount = max(quickAttackDuration[:playerIndex] + quickAttackDuration[playerIndex + 1:])

        if currentPlayerSinguityCount == -1:
            return -1
        elif maxEnemySinguityCount == -1:
            return 1
        else:
            return (maxEnemySinguityCount - currentPlayerSinguityCount) / (currentPlayerSinguityCount + maxEnemySinguityCount + 1)

    @staticmethod
    def quickAttackDuration(gameState: DiscreteGameState, playerId: str) -> int:
        ownPlayer = gameState.playerDictionary[playerId]
        ownSpawners = [s for s in gameState.spawners if s.isClaimed() and s.isAllegedToPlayer(playerId)]

        if len(ownSpawners) == 0:
            return -1

        ownGestationFrames = [s.frameCountBeforeGestationIsDone(gameState.frameCount) for s in ownSpawners]

        claimedEnemySpawners = [s for s in gameState.spawners if s.isClaimed() and not s.isAllegedToPlayer(playerId)]
        totalEnemySpawnerHealthPoints = np.sum([s.getHealthPoints() for s in claimedEnemySpawners])

        if ownPlayer.singuityCount == 0:
            ownForceMeanPosition = np.mean([s.position for s in ownSpawners])
            ownForceStd = 1000
        else:
            ownForceMeanPosition = ownPlayer.singuitiesMeanPosition
            ownForceStd = ownPlayer.singuitiesStd

        return PhysicsEstimator.estimateSpawnerToZeroHealthDurationIntegral(ownPlayer.singuityCount, totalEnemySpawnerHealthPoints, ownGestationFrames, False)\
            + np.sum([PhysicsEstimator.estimateMovementDuration(ownForceMeanPosition, s.position, ownForceStd) for s in claimedEnemySpawners])
