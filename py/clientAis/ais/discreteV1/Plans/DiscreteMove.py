from __future__ import annotations

from typing import Optional

import numpy as np

class DiscreteMove:
    def __init__(self, playerId: str, spawnerId: Optional[str], position: Optional[np.ndarray]):
        self.playerId = playerId
        self.spawnerId = spawnerId
        self.position = position

    @staticmethod
    def fromSpawner(playerId: str, spawnerId: str) -> DiscreteMove:
        return DiscreteMove(playerId, spawnerId, None)

    @staticmethod
    def fromPosition(playerId: str, position: np.ndarray) -> DiscreteMove:
        return DiscreteMove(playerId, None, position)
