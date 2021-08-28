import math
from typing import List, Optional, Tuple, Union

import numpy as np

from clientAis.games.GameState import Singuity, Spawner
from utils import arrays

class PhysicsEstimator:
    FULL_STOP_DURATION = Singuity.MAXIMUM_SPEED_UNITS_PER_FRAME / Singuity.MAXIMUM_ACCELERATION_PER_FRAME
    SINGUITY_ATTACK_PER_FRAME = Singuity.ATTACK_STRENGTH / Singuity.ATTACK_FRAME_LAG
    SPAWNER_SPAWN_PER_FRAME = 1 / Spawner.SPAWN_FRAME_LAG
    MAXIMUM_SINGUITY_DENSITY_PER_AREA = 100 / (math.pi * 18 ** 2)
    MAXIMUM_INTERACTION_DURATION = 2 * 5000 / Singuity.MAXIMUM_SPEED_UNITS_PER_FRAME

    @staticmethod
    def getMinimumStd(singuityCount: int) -> float:
        return max(1., math.sqrt(singuityCount / (math.pi * PhysicsEstimator.MAXIMUM_SINGUITY_DENSITY_PER_AREA)))

    @staticmethod
    def distanceToSpawningSinguities(distance: float) -> int:
        return distance * PhysicsEstimator.SPAWNER_SPAWN_PER_FRAME / Singuity.MAXIMUM_SPEED_UNITS_PER_FRAME

    @staticmethod
    def estimateMovementDuration(singuitiesPosition: np.ndarray, targetPosition: np.ndarray, clusterStd: float = None, acceptableSinguityCount: Tuple[int, int] = None) -> int:
        if clusterStd is None:
            return PhysicsEstimator.distance(targetPosition, singuitiesPosition) / Singuity.MAXIMUM_SPEED_UNITS_PER_FRAME

        if acceptableSinguityCount is None or acceptableSinguityCount[0] < acceptableSinguityCount[1]:
            return (PhysicsEstimator.distance(targetPosition, singuitiesPosition) + clusterStd * 2) / Singuity.MAXIMUM_SPEED_UNITS_PER_FRAME

        distanceToClusterCenter = PhysicsEstimator.distance(targetPosition, singuitiesPosition)
        clusterRearRatio = acceptableSinguityCount[1] / acceptableSinguityCount[0]
        return ((1 - clusterRearRatio) * max(distanceToClusterCenter - clusterStd * 2, 0) + clusterRearRatio * (distanceToClusterCenter + clusterStd * 2)) / Singuity.MAXIMUM_SPEED_UNITS_PER_FRAME

    @staticmethod
    def estimateSpawnerToZeroHealthDuration(singuityCount: int, spawnerHealthPoints: float, spawnerCount: int = 0) -> int:
        if spawnerCount == 0:

            if singuityCount == 0:
                return Spawner.MAX_HEALTH_POINTS ** 2 / PhysicsEstimator.SINGUITY_ATTACK_PER_FRAME

            return spawnerHealthPoints / (singuityCount * PhysicsEstimator.SINGUITY_ATTACK_PER_FRAME)

        c = -spawnerHealthPoints / PhysicsEstimator.SINGUITY_ATTACK_PER_FRAME
        b = singuityCount
        a = spawnerCount / (Spawner.SPAWN_FRAME_LAG * 2)
        return (-b + np.sqrt(b ** 2 - 4 * a * c)) / (2 * a)

    @staticmethod
    def estimateRemainingHealthAfterNFrame(
        singuityCount: int,
        spawnerHealthPoints: float,
        frameCount: int,
        spawnerCount: int = 0,
        returnNewSinguityCount=False
    ) -> Union[float, Tuple[float, int]]:
        remainingHealth = spawnerHealthPoints - PhysicsEstimator.SINGUITY_ATTACK_PER_FRAME * (
                    singuityCount * frameCount + ((spawnerCount / Spawner.SPAWN_FRAME_LAG) * frameCount ** 2) / 2)

        if returnNewSinguityCount:
            newSinguityCount = singuityCount * frameCount + (spawnerCount / Spawner.SPAWN_FRAME_LAG) * frameCount ** 2
            return remainingHealth, newSinguityCount

        return remainingHealth

    @staticmethod
    def estimateSpawnerToZeroHealthDurationIntegral(
        singuityCount: int,
        spawnerHealthPoints: float,
        ownSpawnerGestationFrames: List[int],
        returnNewSinguityCount=False
    ) -> Union[int, Tuple[int, int]]:
        if spawnerHealthPoints == 0:
            return 0

        frameCount = 0

        for spawnerCount, frameCountBeforeNextSpawnerGestationIsDone in enumerate(np.sort(ownSpawnerGestationFrames)):

            if frameCountBeforeNextSpawnerGestationIsDone <= frameCount:
                continue

            frameCountUntilNextSpawnerGestationIsDone = frameCountBeforeNextSpawnerGestationIsDone - frameCount
            newEnemySpawnerHealthPoints, newOwnSinguityCount = PhysicsEstimator.estimateRemainingHealthAfterNFrame(
                singuityCount,
                spawnerHealthPoints,
                frameCountUntilNextSpawnerGestationIsDone,
                spawnerCount,
                returnNewSinguityCount=True
            )

            if newEnemySpawnerHealthPoints <= 0:
                return frameCount + PhysicsEstimator.estimateSpawnerToZeroHealthDuration(singuityCount, spawnerHealthPoints, spawnerCount)

            frameCount += frameCountUntilNextSpawnerGestationIsDone
            spawnerHealthPoints = newEnemySpawnerHealthPoints
            singuityCount = newOwnSinguityCount

        remainingDuration = PhysicsEstimator.estimateSpawnerToZeroHealthDuration(singuityCount, spawnerHealthPoints, len(ownSpawnerGestationFrames))

        if returnNewSinguityCount:
            return singuityCount + remainingDuration * PhysicsEstimator.SPAWNER_SPAWN_PER_FRAME, frameCount + remainingDuration
        return frameCount + remainingDuration

    @staticmethod
    def distance(position1: np.ndarray, position2: np.ndarray) -> float:
        return np.linalg.norm(position1 - position2)

    @staticmethod
    def areSinguitiesColliding(position1: np.ndarray, position2: np.ndarray, clustersStd: float = 0):
        return np.linalg.norm(position1 - position2) <= 2 * Singuity.ATTACK_RANGE + 2 * clustersStd

    @staticmethod
    def getClusterForce(singuityCount: int, clusterStd: float, averageHealth: float) -> float:
        return singuityCount ** 2 * averageHealth / (math.pi * clusterStd ** 2 * PhysicsEstimator.MAXIMUM_SINGUITY_DENSITY_PER_AREA)

    @staticmethod
    def clusterForceToCount(clusterForce: float, clusterStd: float, averageHealth: float) -> int:
        if averageHealth == 0:
            return 0
        return round(math.sqrt(clusterForce * math.pi * clusterStd ** 2 * PhysicsEstimator.MAXIMUM_SINGUITY_DENSITY_PER_AREA / averageHealth))

    # Takes in a list of (singuityCount, clusterStd, singuityAverageHealth) and returns a list of singuity count
    @staticmethod
    def estimateVoidFight(clusters: List[Tuple[int, float, float]]) -> List[int]:
        # Todo void fights also take a minimum amount of time.
        clusterForces = [PhysicsEstimator.getClusterForce(*cluster) for cluster in clusters]
        maximalForceClusterIndex = np.argmax(clusterForces)
        averageAdversaryForce = (np.sum(clusterForces) - clusterForces[maximalForceClusterIndex]) / (len(clusters) - 1)

        maximalRemainingForce = (clusterForces[maximalForceClusterIndex] - clusterForces[maximalForceClusterIndex] / min(
            Singuity.MAX_HEALTH_POINT / Singuity.ATTACK_STRENGTH, clusterForces[maximalForceClusterIndex] / averageAdversaryForce
        ) ** 2) if averageAdversaryForce > 1 else clusterForces[maximalForceClusterIndex]

        return arrays.assign(
            np.zeros(len(clusters)),
            maximalForceClusterIndex,
            PhysicsEstimator.clusterForceToCount(maximalRemainingForce, clusters[maximalForceClusterIndex][1], clusters[maximalForceClusterIndex][2])
        )

    # Takes in clusters as a list of (singuityCount, clusterStd, singuityAverageHealth) and returns a tuple of (spawnerRemainingHealth, interactionDuration, [singuityCount])
    @staticmethod
    def estimateFightOverSpawner(allegedPlayerId: Optional[str], spawnerRemainingHealth: float, restrictedDuration: Optional[int], clusters: List[Tuple[str, int, float, float]]) ->\
    Tuple[float, int, List[int]]:
        clusterForces = [PhysicsEstimator.getClusterForce(singuityCount, averageHealth, clusterStd) for _, singuityCount, averageHealth, clusterStd in clusters]
        homeClusterIndex = None

        if allegedPlayerId is not None:
            for clusterIndex, (cluster, clusterForce) in enumerate(zip(clusters, clusterForces)):
                if cluster[0] == allegedPlayerId:
                    clusterForces[clusterIndex] *= spawnerRemainingHealth / Spawner.MAX_HEALTH_POINTS + 1
                    homeClusterIndex = clusterIndex
                    break

        maximalForceClusterIndex = np.argmax(clusterForces)

        if len(clusters) == 1:
            remainingCounts = np.array([clusters[0][1]])
        else:
            averageAdversaryForce = (np.sum(clusterForces) - clusterForces[maximalForceClusterIndex]) / (len(clusters) - 1)
            maximalRemainingForce = (clusterForces[maximalForceClusterIndex] - clusterForces[maximalForceClusterIndex] / min(
                Singuity.MAX_HEALTH_POINT / Singuity.ATTACK_STRENGTH, clusterForces[maximalForceClusterIndex] / averageAdversaryForce
                ) ** 2) if averageAdversaryForce > 1 else clusterForces[maximalForceClusterIndex]

            if homeClusterIndex == maximalForceClusterIndex:
                maximalRemainingForce /= spawnerRemainingHealth / Spawner.MAX_HEALTH_POINTS + 1

            remainingCounts = arrays.assign(
                np.zeros(len(clusters)), maximalForceClusterIndex, PhysicsEstimator.clusterForceToCount(
                    maximalRemainingForce, clusters[maximalForceClusterIndex][2], clusters[maximalForceClusterIndex][3]
                    )
                )

        if maximalForceClusterIndex != homeClusterIndex:
            interactionMaxDuration = PhysicsEstimator.estimateSpawnerToZeroHealthDuration(remainingCounts[maximalForceClusterIndex], spawnerRemainingHealth)
            if restrictedDuration is None or interactionMaxDuration <= restrictedDuration:
                return 0, interactionMaxDuration, remainingCounts
            else:
                remainingHealth = PhysicsEstimator.estimateRemainingHealthAfterNFrame(remainingCounts[maximalForceClusterIndex], spawnerRemainingHealth, restrictedDuration)
                return remainingHealth, restrictedDuration, remainingCounts
        else:
            return spawnerRemainingHealth, 0, remainingCounts
