from __future__ import annotations

from typing import Optional

import numpy as np

from clientAis.ais.discreteV1.DiscreteGameState import DiscreteGameState, DiscretePlayer, DiscreteSpawner
from clientAis.ais.discreteV1.PhysicsEstimator import PhysicsEstimator

class DiscreteMove:
    def __init__(self, playerId: str, spawnerId: Optional[str], position: Optional[np.ndarray], totalFrameCount: int):
        self.playerId = playerId
        self.spawnerId = spawnerId
        self.position = position
        self.totalFrameCount = totalFrameCount

    @staticmethod
    def fromSpawner(gameState: DiscreteGameState, player: DiscretePlayer, spawner: DiscreteSpawner) -> DiscreteMove:
        if spawner.allegence is None or spawner.isAllegedToPlayer(player.id):
            totalFrameCount = PhysicsEstimator.estimateMovementDuration(player.singuitiesMeanPosition, spawner.position)
        else:
            totalFrameCount = PhysicsEstimator.estimateMovementDuration(player.singuitiesMeanPosition, spawner.position)\
                + PhysicsEstimator.estimateSpawnerToZeroHealthDurationIntegral(
                    player.singuityCount,
                    spawner.getHealthPoints(),
                    [s.frameCountBeforeGestationIsDone(gameState.frameCount) for s in gameState.spawners if s.isClaimed() and s.isAllegedToPlayer(player.id)]
                )

        return DiscreteMove(player.id, spawner.id, None, totalFrameCount)

    @staticmethod
    def fromPosition(player: DiscretePlayer, position: np.ndarray) -> DiscreteMove:
        return DiscreteMove(player.id, None, position, PhysicsEstimator.estimateMovementDuration(player.singuitiesMeanPosition, position))
