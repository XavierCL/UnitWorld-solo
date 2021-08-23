from typing import List, Optional, Tuple, Union

import numpy as np

from clientAis.games.GameState import Singuity, Spawner
from utils import arrays

class PhysicsEstimator:
    SINGUITY_ATTACK_PER_FRAME = Singuity.ATTACK_STRENGTH / Singuity.ATTACK_FRAME_LAG
    SPAWNER_SPAWN_PER_FRAME = 1 / Spawner.SPAWN_FRAME_LAG

    @staticmethod
    def distanceToSpawningSinguities(distance: float) -> int:
        return distance * PhysicsEstimator.SPAWNER_SPAWN_PER_FRAME / Singuity.MAXIMUM_SPEED_UNITS_PER_FRAME

    @staticmethod
    def estimateMovementDuration(singuitiesPosition, targetPosition, clusterStd=None) -> int:
        if clusterStd is None:
            return np.linalg.norm(targetPosition - singuitiesPosition) / Singuity.MAXIMUM_SPEED_UNITS_PER_FRAME

        return (np.linalg.norm(targetPosition - singuitiesPosition) + clusterStd) / Singuity.MAXIMUM_SPEED_UNITS_PER_FRAME

    @staticmethod
    def estimateSpawnerToZeroHealthDuration(singuityCount: int, spawnerHealthPoints: float, spawnerCount: int = 0) -> int:
        if spawnerCount == 0:

            if singuityCount == 0:
                return Spawner.MAX_HEALTH_POINTS**2 / PhysicsEstimator.SINGUITY_ATTACK_PER_FRAME

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
        remainingHealth = spawnerHealthPoints - PhysicsEstimator.SINGUITY_ATTACK_PER_FRAME * (singuityCount * frameCount + ((spawnerCount / Spawner.SPAWN_FRAME_LAG) * frameCount ** 2) / 2)

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
    def areSinguitiesColliding(position1: np.ndarray, position2: np.ndarray):
        return np.linalg.norm(position1 - position2) <= 2 * Singuity.ATTACK_RANGE

    @staticmethod
    def getClusterForce(singuityCount: int, clusterStd: float, averageHealth: float) -> float:
        return singuityCount * averageHealth / clusterStd

    # Takes in a list of (singuityCount, clusterStd, singuityAverageHealth) and returns a list of singuity count
    @staticmethod
    def estimateVoidFight(clusters: List[Tuple[int, float, float]]) -> List[int]:
        clusterForces = [PhysicsEstimator.getClusterForce(*cluster) for cluster in clusters]
        maximalForceClusterIndex = np.argmax(clusterForces)
        averageAdversaryForce = (np.sum(clusterForces) - clusterForces[maximalForceClusterIndex]) / (len(clusters) - 1)
        maximalRemainingForce = maximalForceClusterIndex - maximalForceClusterIndex/min(Singuity.MAX_HEALTH_POINT / Singuity.ATTACK_STRENGTH, maximalForceClusterIndex / averageAdversaryForce)**2
        return arrays.assign(np.zeros(len(clusters)), maximalForceClusterIndex, maximalRemainingForce * clusters[maximalForceClusterIndex][1] / clusters[maximalForceClusterIndex][2])

    # Takes in clusters as a list of (singuityCount, clusterStd, singuityAverageHealth) and returns a tuple of (spawnerRemainingHealth, interactionDuration, [singuityCount])
    @staticmethod
    def estimateFightOverSpawner(allegedPlayerId: Optional[str], spawnerRemainingHealth: float, restrictedDuration, clusters: List[Tuple[int, float, float]]) -> Tuple[float, int, List[int]]:
        todo

