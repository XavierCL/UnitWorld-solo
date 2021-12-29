import math

import numpy as np

from clientAis.ais.discreteV1.models.DiscreteGameState import DiscreteGameState
from clientAis.ais.discreteV1.physics.PhysicsEstimator import PhysicsEstimator
from clientAis.ais.discreteV1.searches.StateScore import StateScore
from utils import arrays

class DiscreteGameScorer:
    # Returns -1 when all is lost for the current player
    @staticmethod
    def score(gameState: DiscreteGameState, playerId: str) -> StateScore:
        playerIds = list(gameState.playerDictionary.keys())
        playerIndex = arrays.firstIndex(playerIds, playerId)

        if playerIndex == -1:
            return StateScore.loss(gameState.frameCount)

        if len(gameState.playerDictionary) < 2 and playerId in gameState.playerDictionary:
            return StateScore.win(gameState.frameCount)
        elif len(gameState.playerDictionary) < 2:
            return StateScore.loss(gameState.frameCount)

        quickAttackDuration = [DiscreteGameScorer.quickAttackDuration(gameState, p) for p in playerIds]
        currentPlayerQuickAttackDuration = quickAttackDuration[playerIndex]
        bestEnemyQuickAttackDuration = min(quickAttackDuration[:playerIndex] + quickAttackDuration[playerIndex + 1:])

        if currentPlayerQuickAttackDuration == -1:
            return StateScore.loss(gameState.frameCount)
        elif bestEnemyQuickAttackDuration == -1:
            return StateScore.win(gameState.frameCount)
        elif currentPlayerQuickAttackDuration < bestEnemyQuickAttackDuration:
            return StateScore.win(currentPlayerQuickAttackDuration)
        elif currentPlayerQuickAttackDuration > bestEnemyQuickAttackDuration:
            return StateScore.loss(bestEnemyQuickAttackDuration)

        return StateScore.tie(currentPlayerQuickAttackDuration)

    @staticmethod
    def quickAttackDuration(gameState: DiscreteGameState, playerId: str) -> int:
        ownPlayer = gameState.playerDictionary[playerId]
        ownSpawners = [s for s in gameState.spawners if s.isClaimed() and s.isAllegedToPlayer(playerId)]

        if len(ownSpawners) == 0:
            return -1

        ownGestationFrames = [s.frameCountBeforeGestationIsDone(gameState.frameCount) for s in ownSpawners]

        claimedEnemySpawners = [s for s in gameState.spawners if s.isClaimed() and not s.isAllegedToPlayer(playerId)]
        totalEnemySpawnerHealthPoints = np.sum([s.getHealthPoints() for s in claimedEnemySpawners])
        totalEnemyCount = np.sum([p.singuityCount for p in gameState.playerDictionary.values() if p.id != playerId])
        virtualSinguityCount = np.max([ownPlayer.singuityCount - totalEnemyCount * 1.1, 1])

        if ownPlayer.singuityCount == 0:
            ownForceMeanPosition = np.mean([s.position for s in ownSpawners], axis=0)
            ownForceStd = 1000
        else:
            ownForceMeanPosition = ownPlayer.singuitiesMeanPosition
            ownForceStd = ownPlayer.singuitiesStd

        return PhysicsEstimator.estimateSpawnerToZeroHealthDurationIntegral(virtualSinguityCount, totalEnemySpawnerHealthPoints, [], False, 1.0) * 5\
            + np.sum([PhysicsEstimator.estimateMovementDuration(ownForceMeanPosition, s.position, ownForceStd) for s in claimedEnemySpawners]) * 5\
            + gameState.frameCount
