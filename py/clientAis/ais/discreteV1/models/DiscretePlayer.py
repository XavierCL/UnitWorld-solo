from __future__ import annotations

from typing import List, Optional

import numpy as np
from sklearn.cluster import DBSCAN

from clientAis.ais.discreteV1.physics.PhysicsEstimator import PhysicsEstimator
from clientAis.ais.discreteV1.Plans.DiscreteMove import DiscreteMove
from clientAis.games.GameState import GameState, Singuity, Spawner
from utils import arrays

class DiscretePlayer:
    def __init__(self, id: str, singuityCount: int, singuitiesMeanPosition: np.ndarray, singuitiesStd: float, singuitiesAverageHealth: float, inCluster: Optional[List[str]] = None):
        self.id = id
        self.singuityCount = singuityCount
        self.singuitiesMeanPosition = singuitiesMeanPosition
        self.singuitiesStd = singuitiesStd
        self.singuitiesAverageHealth = singuitiesAverageHealth
        self.inCluster = inCluster

    @staticmethod
    def fromGameState(gameState: GameState, playerId: str) -> DiscretePlayer:
        playerSinguities = [s for s in gameState.singuities if s.playerId == playerId]

        if len(playerSinguities) == 0:
            return DiscretePlayer(playerId, 0, np.zeros(2), PhysicsEstimator.getMinimumStd(0), 0, [])

        singuityPositions = [s.position for s in playerSinguities]

        singuityClusterIds = DBSCAN(eps=100, min_samples=25).fit_predict(singuityPositions)
        singuityClusters: List[List[Singuity]] = [[] for _ in range(np.max(singuityClusterIds) + 1)]

        for index, singuity in [(index, singuity) for index, singuity in enumerate(playerSinguities) if singuityClusterIds[index] != -1]:
            singuityClusters[singuityClusterIds[index]].append(singuity)

        def getClusterForce(singuities: List[Singuity]) -> float:
            clusterStd = arrays.mad([s.position for s in singuities], axis=0, minMad=PhysicsEstimator.getMinimumStd(len(singuities)))
            averageHealth = np.mean([s.healthPoints for s in singuities]).item()
            return PhysicsEstimator.getClusterForce(len(singuities), np.linalg.norm(clusterStd), averageHealth)

        clusterForces = [getClusterForce(cluster) for cluster in singuityClusters]

        if len(clusterForces) == 0:
            singuityStd, singuityCenter = arrays.mad(singuityPositions, axis=0, returnMedian=True, minMad=PhysicsEstimator.getMinimumStd(len(singuityPositions)))
            singuityAverageHealth = np.mean([s.healthPoints for s in playerSinguities]).item()
            return DiscretePlayer(playerId, len(playerSinguities), singuityCenter, np.linalg.norm(singuityStd), singuityAverageHealth, [s.id for s in playerSinguities])

        else:
            keptCluster = singuityClusters[np.argmax(clusterForces)]

            singuityStd, singuityCenter = arrays.mad([s.position for s in keptCluster], axis=0, returnMedian=True, minMad=PhysicsEstimator.getMinimumStd(len(keptCluster)))
            singuityAverageHealth = np.mean([s.healthPoints for s in keptCluster]).item()
            return DiscretePlayer(playerId, len(keptCluster), singuityCenter, np.linalg.norm(singuityStd), singuityAverageHealth, [s.id for s in keptCluster])

    def executeMovement(self, move: DiscreteMove, targetPosition: np.ndarray, restrictedDuration: int = None, acceptableSinguityCount: int = None):
        def getLastingMoveDuration(moveDuration: int) -> int:
            minimumBoundDuration = moveDuration if move.minimumMoveTime is None else max(moveDuration, move.minimumMoveTime)
            maximumBoundDuration = minimumBoundDuration if restrictedDuration is None else min(minimumBoundDuration, restrictedDuration)
            return maximumBoundDuration

        originalMovementDuration = PhysicsEstimator.estimateMovementDuration(self.singuitiesMeanPosition, targetPosition, clusterStd=self.singuitiesStd, acceptableSinguityCount=None if acceptableSinguityCount is None else (self.singuityCount, acceptableSinguityCount))
        constrainedMovementDuration = getLastingMoveDuration(originalMovementDuration)

        if constrainedMovementDuration == 0:
            return 0, self

        targetIsWithinStd = PhysicsEstimator.distance(self.singuitiesMeanPosition, targetPosition) <= self.singuitiesStd

        if targetIsWithinStd:
            ratioOfTimeSpentInside = min(constrainedMovementDuration * Singuity.MAXIMUM_SPEED_UNITS_PER_FRAME, self.singuitiesStd * 2) / (self.singuitiesStd * 2)
            newStd = min(PhysicsEstimator.getMinimumStd(self.singuityCount) * ratioOfTimeSpentInside + self.singuitiesStd * (1 - ratioOfTimeSpentInside), self.singuitiesStd)
        else:
            # maximum duration on a 5000 unit map
            newStd = self.singuitiesStd * (1 + constrainedMovementDuration / 1768)

        return constrainedMovementDuration,\
            DiscretePlayer(
                self.id,
                self.singuityCount,
                self.singuitiesMeanPosition + (targetPosition - self.singuitiesMeanPosition) * (constrainedMovementDuration / constrainedMovementDuration),
                newStd,
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

        if PhysicsEstimator.distance(spawnerPosition, self.singuitiesMeanPosition) <= self.singuitiesStd * 2:
            newSinguitiesMean, newSinguitiesStd, newSinguitiesCount = arrays.combineMeanStdAndCount(
                self.singuitiesMeanPosition, np.sqrt(atTargetCount), atTargetCount, unitPositionInLine, singuitiesInLineStd, unitCountInLine, PhysicsEstimator.getMinimumStd(atTargetCount + unitCountInLine)
            )
        else:
            newSinguitiesMean, newSinguitiesStd, newSinguitiesCount = self.singuitiesMeanPosition, np.sqrt(atTargetCount), atTargetCount

        singuitiesMean, singuitiesStd, singuitiesCount = arrays.combineMeanStdAndCount(
            self.singuitiesMeanPosition, np.array([self.singuitiesStd]), self.singuityCount, newSinguitiesMean, np.linalg.norm(newSinguitiesStd), newSinguitiesCount, PhysicsEstimator.getMinimumStd(newSinguitiesCount + self.singuityCount)
        )

        return DiscretePlayer(self.id, singuitiesCount, singuitiesMean, np.linalg.norm(singuitiesStd), 0 if self.singuityCount + newSinguitiesCount == 0 else (self.singuitiesAverageHealth * self.singuityCount + Singuity.MAX_HEALTH_POINT * newSinguitiesCount) / (self.singuityCount + newSinguitiesCount))
