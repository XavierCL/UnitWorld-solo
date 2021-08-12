from typing import List, Tuple, Union

import numpy as np

from clientAis.games.GameState import Singuity, Spawner

class PhysicsEstimator:
    SINGUITY_ATTACK_PER_FRAME = Singuity.ATTACK_STRENGTH / Singuity.ATTACK_FRAME_LAG

    @staticmethod
    def estimateMovementDuration(singuitiesPosition, targetPosition) -> int:
        return np.linalg.norm(targetPosition - singuitiesPosition) / Singuity.MAXIMUM_UNITS_PER_FRAME

    @staticmethod
    def estimateSpawnerToZeroHealthDuration(singuityCount: int, spawnerHealthPoints: float, spawnerCount: int = 0) -> int:
        if spawnerCount == 0:
            return spawnerHealthPoints / (singuityCount * PhysicsEstimator.SINGUITY_ATTACK_PER_FRAME)

        c = -spawnerHealthPoints/PhysicsEstimator.SINGUITY_ATTACK_PER_FRAME
        b = singuityCount
        a = spawnerCount / (Spawner.SPAWN_FRAME_LAG * 2)
        return (-b + np.sqrt(b**2 - 4*a*c)) / (2 * a)

    @staticmethod
    def estimateRemainingHealthAfterNFrame(singuityCount: int, spawnerHealthPoints: float, frameCount: int, spawnerCount: int = 0, returnNewSinguityCount = False) -> Union[float, Tuple[float, int]]:
        remainingHealth = spawnerHealthPoints - PhysicsEstimator.SINGUITY_ATTACK_PER_FRAME * (singuityCount * frameCount + ((spawnerCount / Spawner.SPAWN_FRAME_LAG) * frameCount ** 2) / 2)

        if returnNewSinguityCount:
            newSinguityCount = singuityCount * frameCount + (spawnerCount / Spawner.SPAWN_FRAME_LAG) * frameCount ** 2
            return remainingHealth, newSinguityCount

        return remainingHealth

    @staticmethod
    def estimateSpawnerToZeroHealthDurationIntegral(singuityCount: int, spawnerHealthPoints: float, ownSpawnerGestationFrames: List[int]) -> int:
        if spawnerHealthPoints == 0:
            return 0

        frameCount = 0

        for spawnerCount, frameCountBeforeNextSpawnerGestationIsDone in enumerate(np.sort(ownSpawnerGestationFrames)):

            if frameCountBeforeNextSpawnerGestationIsDone <= frameCount:
                continue

            frameCountUntilNextSpawnerGestationIsDone = frameCountBeforeNextSpawnerGestationIsDone - frameCount
            newEnemySpawnerHealthPoints, newOwnSinguityCount = PhysicsEstimator.estimateRemainingHealthAfterNFrame(singuityCount, spawnerHealthPoints, frameCountUntilNextSpawnerGestationIsDone, spawnerCount, returnNewSinguityCount=True)

            if newEnemySpawnerHealthPoints <= 0:
                return frameCount + PhysicsEstimator.estimateSpawnerToZeroHealthDuration(singuityCount, spawnerHealthPoints, spawnerCount)

            frameCount += frameCountUntilNextSpawnerGestationIsDone
            spawnerHealthPoints = newEnemySpawnerHealthPoints
            singuityCount = newOwnSinguityCount

        return frameCount + PhysicsEstimator.estimateSpawnerToZeroHealthDuration(singuityCount, spawnerHealthPoints, len(ownSpawnerGestationFrames))