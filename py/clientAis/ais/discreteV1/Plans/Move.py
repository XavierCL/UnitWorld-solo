from __future__ import annotations

from typing import List, Optional

import numpy as np

from clientAis.games.GameState import Spawner

class Move:
    def __init__(self, targetsPosition: bool, singuityIds: List[str], spawnerId: Optional[str], position: np.ndarray):
        self.targetsPosition = targetsPosition
        self.singuityIds = singuityIds
        self.spawnerId = spawnerId
        self.position = position

    @staticmethod
    def fromPosition(singuityIds: List[str], position: np.ndarray) -> Move:
        return Move(True, singuityIds, None, position)

    @staticmethod
    def fromSpawner(singuityIds: List[str], spawner: Spawner, currentPlayerId: str) -> Move:
        if spawner.allegence is None or not spawner.allegence.isClaimed and spawner.allegence.playerId == currentPlayerId:
            return Move(False, singuityIds, spawner.id, spawner.position)
        else:
            return Move(True, singuityIds, spawner.id, spawner.position)

    @staticmethod
    def fromNothing():
        return Move(False, None, None, None)
