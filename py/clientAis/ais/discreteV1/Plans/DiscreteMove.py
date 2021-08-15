from __future__ import annotations

from typing import Optional

import numpy as np

class DiscreteMove:
    def __init__(self, playerId: str, spawnerId: Optional[str], position: Optional[np.ndarray], minimumMoveTime: Optional[int]):
        self.playerId = playerId
        self.spawnerId = spawnerId
        self.position = position
        self.minimumMoveTime = minimumMoveTime

    @staticmethod
    def fromSpawner(playerId: str, spawnerId: str) -> DiscreteMove:
        return DiscreteMove(playerId, spawnerId, None, None)

    @staticmethod
    def fromPosition(playerId: str, position: np.ndarray, timeSpentAtPosition: Optional[int] = None) -> DiscreteMove:
        return DiscreteMove(playerId, None, position, timeSpentAtPosition)

    @staticmethod
    def fromNothing():
        return DiscreteMove(None, None, None, None)
