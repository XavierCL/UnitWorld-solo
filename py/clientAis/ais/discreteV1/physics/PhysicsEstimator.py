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
    ATTACKS_NEEDED_TO_KILL_ONE_SINGUITY = Singuity.MAX_HEALTH_POINT / Singuity.ATTACK_STRENGTH
    FRAMES_NEEDED_TO_KILL_ONE_SINGUITY = Singuity.ATTACK_FRAME_LAG * ATTACKS_NEEDED_TO_KILL_ONE_SINGUITY

    @staticmethod
    def getMinimumStd(singuityCount: Union[np.ndarray, int]) -> Union[np.ndarray, float]:
        return np.max([np.ones_like(singuityCount), np.sqrt(singuityCount / (math.pi * PhysicsEstimator.MAXIMUM_SINGUITY_DENSITY_PER_AREA))], axis=0)

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
        return ((1 - clusterRearRatio) * max(distanceToClusterCenter - clusterStd * 2, 0) + clusterRearRatio * (
                    distanceToClusterCenter + clusterStd * 2)) / Singuity.MAXIMUM_SPEED_UNITS_PER_FRAME

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
        return np.linalg.norm(position1 - position2, axis=-1)

    @staticmethod
    def distance2(position1: np.ndarray, position2: np.ndarray) -> float:
        return np.sum((position1 - position2) ** 2, axis=-1)

    @staticmethod
    def areSinguitiesColliding(position1: np.ndarray, position2: np.ndarray, clustersStd: float = 0, returnDistance=False) -> Union[bool, Tuple[bool, float]]:
        distance2 = PhysicsEstimator.distance2(position1, position2)
        isColliding = distance2 <= Singuity.ATTACK_RANGE + clustersStd

        if returnDistance:
            return (isColliding, distance2)

        return isColliding

    @staticmethod
    def getClusterForce(singuityCount: int, clusterStd: float, averageHealth: float) -> float:
        return singuityCount ** 2 * averageHealth / (math.pi * clusterStd ** 2 * PhysicsEstimator.MAXIMUM_SINGUITY_DENSITY_PER_AREA)

    @staticmethod
    def clusterForceToCount(clusterForce: float, clusterStd: float, averageHealth: float) -> int:
        if averageHealth == 0:
            return 0
        return round(math.sqrt(clusterForce * math.pi * clusterStd ** 2 * PhysicsEstimator.MAXIMUM_SINGUITY_DENSITY_PER_AREA / averageHealth))

    # Asymptotically correct estimation of a fight between two enemy clusters where they have the same std
    # Parameter is 2 * (number of units, average unit health), output is (fight duration, cluster1 remaining units, cluster2 remaining units)
    # Never will the two clusters be returned with remaining units.
    # However both clusters can be returned with 0 units if they have the same fighting force
    @staticmethod
    def estimateVoid1on1FightWithSameStd(cluster1: Tuple[int, float], cluster2: Tuple[int, float]) -> Tuple[int, int, int]:
        (cluster1Units, cluster1Healths), (cluster2Units, cluster2Healths) = cluster1, cluster2
        (cluster1Units, cluster2Units) = (math.ceil(cluster1Units), math.ceil(cluster2Units))

        if cluster1Units <= 0 or cluster1Healths <= 0:
            return 0, 0, cluster2Units
        elif cluster2Units <= 0 or cluster2Healths <= 0:
            return 0, cluster1Units, 0

        cluster1HealthRoundFactor, cluster2HealthRoundFactor = PhysicsEstimator.ATTACKS_NEEDED_TO_KILL_ONE_SINGUITY * cluster1Units, PhysicsEstimator.ATTACKS_NEEDED_TO_KILL_ONE_SINGUITY * cluster2Units
        (cluster1Healths, cluster2Healths) = (
        math.ceil(cluster1Healths * cluster1HealthRoundFactor) / cluster1HealthRoundFactor, math.ceil(cluster2Healths * cluster2HealthRoundFactor) / cluster2HealthRoundFactor)

        if cluster1Healths <= 0:
            return 0, 0, cluster2Units
        elif cluster2Healths <= 0:
            return 0, cluster1Units, 0

        healthsSqrt = math.sqrt(cluster1Healths * cluster2Healths)
        discreteCheck = cluster2Units * healthsSqrt - cluster1Healths * cluster1Units

        if discreteCheck == 0:
            # Tie, solve for unit count == 1
            # One attempt at https://www.desmos.com/calculator/pl2aczcp4m
            # Alternate solution, solve for cluster1 + 1
            approximateSolution = PhysicsEstimator.estimateVoid1on1FightWithSameStd((cluster1Units + 1, cluster1Healths), cluster2)
            return approximateSolution[0], 0, 0

        cluster1Wins = discreteCheck <= 0

        def getFightDuration(winnerUnits, winnerHealths, loserUnits):
            return PhysicsEstimator.FRAMES_NEEDED_TO_KILL_ONE_SINGUITY * healthsSqrt * math.log(
                (loserUnits * healthsSqrt + winnerHealths * winnerUnits) / (loserUnits * healthsSqrt - winnerHealths * winnerUnits)
                ) / 2

        fightDuration = getFightDuration(cluster1Units, cluster1Healths, cluster2Units) if cluster1Wins else getFightDuration(cluster2Units, cluster2Healths, cluster1Units)

        def getRemainingUnits(winnerUnits, winnerHealths, loserUnits, atFrame):
            return math.ceil(
                ((winnerHealths * winnerUnits - loserUnits * healthsSqrt) * math.e ** (atFrame / (PhysicsEstimator.FRAMES_NEEDED_TO_KILL_ONE_SINGUITY * healthsSqrt)) + (
                            winnerHealths * winnerUnits + loserUnits * healthsSqrt) * math.e ** (
                             -atFrame / (PhysicsEstimator.FRAMES_NEEDED_TO_KILL_ONE_SINGUITY * healthsSqrt))) / (2 * winnerHealths)
                )

        return (math.ceil(fightDuration), getRemainingUnits(cluster1Units, cluster1Healths, cluster2Units, fightDuration), 0)\
            if cluster1Wins\
            else (math.ceil(fightDuration), 0, getRemainingUnits(cluster2Units, cluster2Healths, cluster1Units, fightDuration))

    # Tales in 2*[unit count, std, average health]
    # Returns the frame duration, unit count for cluster 1 and unit count for cluster 2.
    # Average health is assumed not to have changed.
    @staticmethod
    def estimateVoid1on1Fight(cluster1: Tuple[int, float, float], cluster2: Tuple[int, float, float]) -> Tuple[int, int, int]:
        (cluster1Units, cluster1Health, cluster1Std), (cluster2Units, cluster2Health, cluster2Std) = cluster1, cluster2

        def estimateVoid1on1FightFromStd(smallestCluster: Tuple[int, float, float], largestCluster: Tuple[int, float, float]) -> Tuple[int, int, int]:
            (smallestClusterUnits, smallestClusterHealth, smallestClusterStd), (largestClusterUnits, largestClusterHealth, largestClusterStd) = smallestCluster, largestCluster

            equivalentLargestClusterUnits = round(largestClusterUnits * smallestClusterStd / largestClusterStd)
            equivalentFightResult = PhysicsEstimator.estimateVoid1on1FightWithSameStd(
                (smallestClusterUnits, smallestClusterHealth), (equivalentLargestClusterUnits, largestClusterHealth)
            )

            if equivalentFightResult[2] > 0:
                return equivalentFightResult[0], 0, largestClusterUnits - equivalentLargestClusterUnits + equivalentFightResult[2]

            smallestLostRatio = equivalentFightResult[1] / smallestClusterUnits
            largestLostRatio = (largestClusterUnits - equivalentLargestClusterUnits + equivalentFightResult[2]) / largestClusterUnits

            if smallestLostRatio < largestLostRatio:
                fightDuration = math.ceil(equivalentFightResult[0] / (1 - smallestLostRatio))
                return fightDuration, 0, math.ceil(largestClusterUnits - (equivalentLargestClusterUnits - equivalentFightResult[2]) / (1 - smallestLostRatio))
            else:
                fightDuration = math.ceil(equivalentFightResult[0] / (1 - largestLostRatio))
                return fightDuration, math.ceil(smallestClusterUnits - (smallestClusterUnits - equivalentFightResult[1]) / (1 - largestLostRatio)), 0

        if cluster1Std <= cluster2Std:
            return estimateVoid1on1FightFromStd(cluster1, cluster2)
        else:
            duration, cluster2Remaining, cluster1Remaining = estimateVoid1on1FightFromStd(cluster2, cluster1)
            return duration, cluster1Remaining, cluster2Remaining

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
                    clusterForces[clusterIndex] *= 10 * spawnerRemainingHealth / Spawner.MAX_HEALTH_POINTS + 1
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
                maximalRemainingForce /= 10 * spawnerRemainingHealth / Spawner.MAX_HEALTH_POINTS + 1

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
