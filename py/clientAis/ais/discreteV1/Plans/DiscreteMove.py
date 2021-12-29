from __future__ import annotations

from typing import Optional

import numpy as np

class DiscreteMove:
    def __init__(self, debugName: str, playerId: str, spawnerId: Optional[str], position: Optional[np.ndarray], minimumMoveTime: Optional[int]):
        self.debugName = debugName
        self.playerId = playerId
        self.spawnerId = spawnerId
        self.position = position
        self.minimumMoveTime = minimumMoveTime

    @staticmethod
    def fromSpawner(debugSpawnerType: str, playerId: str, spawnerId: str, timeSpentAtSpawner: Optional[int] = None) -> DiscreteMove:
        return DiscreteMove(f"to {debugSpawnerType} spawner {spawnerId}", playerId, spawnerId, None, timeSpentAtSpawner)

    @staticmethod
    def fromPosition(debugIsCurrentCluster: bool, playerId: str, position: np.ndarray, timeSpentAtPosition: Optional[int] = None) -> DiscreteMove:
        return DiscreteMove(f"to {'own' if debugIsCurrentCluster else 'enemy'} cluster", playerId, None, position, timeSpentAtPosition)

    @staticmethod
    def fromNothing():
        return DiscreteMove("nothing", None, None, None, None)
