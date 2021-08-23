from __future__ import annotations

import numpy as np

from clientAis.ais.discreteV1.PhysicsEstimator import PhysicsEstimator
from clientAis.ais.discreteV1.Plans.DiscreteMove import DiscreteMove
from clientAis.games.GameState import GameState, Singuity, Spawner
from utils import arrays

class DiscretePlayer:
    def __init__(self, id: str, singuityCount: int, singuitiesMeanPosition: np.ndarray, singuitiesStd: float, singuitiesAverageHealth: float):
        self.id = id
        self.singuityCount = singuityCount
        self.singuitiesMeanPosition = singuitiesMeanPosition
        self.singuitiesStd = singuitiesStd
        self.singuitiesAverageHealth = singuitiesAverageHealth

    @staticmethod
    def fromGameState(gameState: GameState, playerId: str) -> DiscretePlayer:
        playerSinguities = [s for s in gameState.singuities if s.playerId == playerId]
        singuityPositions = [s.position for s in playerSinguities]
        singuityCenter = np.median(singuityPositions, axis=0)
        singuityAverageHealth = np.mean(s.healthPoints for s in playerSinguities).item()
        return DiscretePlayer(playerId, len(playerSinguities), singuityCenter, np.linalg.norm(np.std(singuityPositions, axis=0)), singuityAverageHealth)

    def executeMovement(self, move: DiscreteMove, targetPosition: np.ndarray, restrictedDuration: int = None):
        def getLastingMoveDuration(moveDuration: int) -> int:
            minimumBoundDuration = moveDuration if move.minimumMoveTime is None else max(moveDuration, move.minimumMoveTime)
            maximumBoundDuration = minimumBoundDuration if restrictedDuration is None else min(minimumBoundDuration, restrictedDuration)
            return maximumBoundDuration

        originalMovementDuration = PhysicsEstimator.estimateMovementDuration(self.singuitiesMeanPosition, targetPosition, clusterStd=self.singuitiesStd)
        constrainedMovementDuration = getLastingMoveDuration(originalMovementDuration)

        if constrainedMovementDuration == 0:
            return self

        targetIsWithinStd = PhysicsEstimator.distance(self.singuitiesMeanPosition, targetPosition) <= self.singuitiesStd

        return constrainedMovementDuration,\
            DiscretePlayer(
                self.id,
                self.singuityCount,
                self.singuitiesMeanPosition + (targetPosition - self.singuitiesMeanPosition) * (constrainedMovementDuration / constrainedMovementDuration),
                10 / min(constrainedMovementDuration * Singuity.MAXIMUM_SPEED_UNITS_PER_FRAME, self.singuitiesStd * 2) / (self.singuitiesStd * 2) if targetIsWithinStd else self.singuitiesStd,
                self.singuitiesAverageHealth
            )

    def fought(self, remainingSinguityCount):
        return DiscretePlayer(self.id, remainingSinguityCount, self.singuitiesMeanPosition, self.singuitiesStd, self.singuitiesAverageHealth)

    def tryClaimFor(self, remainingSinguitiesToClaim):
        return DiscretePlayer(self.id, max(self.singuityCount - remainingSinguitiesToClaim, 0), self.singuitiesMeanPosition, self.singuitiesStd, self.singuitiesAverageHealth)

    def appendNewSpawned(self, spawnerPosition: np.ndarray, spawnerLastFrameClaimed: int, anteMoveFrameCount: int, postMoveFrameCount: int) -> DiscretePlayer:
        matureSpawnerDuration = min(postMoveFrameCount - anteMoveFrameCount, postMoveFrameCount - spawnerLastFrameClaimed + Spawner.GESTATION_FRAME_LAG)
        durationToTarget = PhysicsEstimator.estimateMovementDuration(spawnerPosition, self.singuitiesMeanPosition)
        atTargetCount = round(max((matureSpawnerDuration - durationToTarget) * PhysicsEstimator.SPAWNER_SPAWN_PER_FRAME, 0))
        distanceFromSpawnerToMeanPosition = np.linalg.norm(self.singuitiesMeanPosition - spawnerPosition)
        maxDistanceFromTime = matureSpawnerDuration * Singuity.MAXIMUM_SPEED_UNITS_PER_FRAME
        furthestSinguityRatio = 1 if distanceFromSpawnerToMeanPosition <= maxDistanceFromTime else maxDistanceFromTime / distanceFromSpawnerToMeanPosition
        unitPositionInLine = (spawnerPosition + (self.singuitiesMeanPosition - spawnerPosition) * furthestSinguityRatio / 2)
        unitCountInLine = round(PhysicsEstimator.distanceToSpawningSinguities(distanceFromSpawnerToMeanPosition * furthestSinguityRatio))
        singuitiesInLineStd = distanceFromSpawnerToMeanPosition * furthestSinguityRatio / 4

        newSinguitiesMean, newSinguitiesStd, newSinguitiesCount = arrays.combineMeanStdAndCount(
            self.singuitiesMeanPosition, np.ones(1), atTargetCount, unitPositionInLine, singuitiesInLineStd, unitCountInLine
        )

        singuitiesMean, singuitiesStd, singuitiesCount = arrays.combineMeanStdAndCount(
            self.singuitiesMeanPosition, np.array([self.singuitiesStd]), self.singuityCount, newSinguitiesMean, np.linalg.norm(newSinguitiesStd), newSinguitiesCount
        )

        return DiscretePlayer(self.id, singuitiesCount, singuitiesMean, np.linalg.norm(singuitiesStd), self.singuitiesAverageHealth * self.singuityCount + Singuity.MAX_HEALTH_POINT * newSinguitiesCount)
