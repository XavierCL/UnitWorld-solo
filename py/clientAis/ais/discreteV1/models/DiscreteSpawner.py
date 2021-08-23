from __future__ import annotations

import math
from typing import Optional

import numpy as np

from clientAis.games.GameState import Spawner, SpawnerAllegence

class DiscreteSpawnerAllegence:
    def __init__(self, isClaimed: bool, playerId: str, healthPoints: float):
        self.isClaimed = isClaimed
        self.playerId = playerId
        self.healthPoints = healthPoints

    @staticmethod
    def fromAllegence(allegence: Optional[SpawnerAllegence]) -> Optional[DiscreteSpawnerAllegence]:
        return None if allegence is None else DiscreteSpawnerAllegence(allegence.isClaimed, allegence.playerId, allegence.healthPoints)

    def loseHealthPointsTo(self, healthPoints):
        if healthPoints <= 0:
            return None

        return DiscreteSpawnerAllegence(self.isClaimed, self.playerId, healthPoints)

class DiscreteSpawner:
    def __init__(self, id: str, position: np.ndarray, allegence: Optional[DiscreteSpawnerAllegence], lastFrameClaimed: int):
        self.id = id
        self.position = position
        self.allegence = allegence
        self.lastFrameClaimed = lastFrameClaimed

    @staticmethod
    def fromSpawner(spawner: Spawner) -> DiscreteSpawner:
        return DiscreteSpawner(spawner.id, spawner.position, DiscreteSpawnerAllegence.fromAllegence(spawner.allegence), spawner.lastClaimFrameCount)

    def isClaimed(self):
        return self.allegence is not None and self.allegence.isClaimed

    def isAllegedToPlayer(self, playerId):
        return self.allegence is not None and self.allegence.playerId == playerId

    def getHealthPoints(self):
        return 0 if self.allegence is None else self.allegence.healthPoints

    def remainingSinguitiesToCapture(self, playerId: str) -> int:
        if self.allegence is None:
            return Spawner.REQUIRED_CAPTURING_SINGUITIES
        elif self.allegence.playerId == playerId:
            if self.allegence.isClaimed:
                return 0
            else:
                return int(math.ceil(Spawner.REQUIRED_CAPTURING_SINGUITIES - Spawner.REQUIRED_CAPTURING_SINGUITIES * self.allegence.healthPoints / Spawner.MAX_HEALTH_POINTS))
        else:
            return Spawner.REQUIRED_CAPTURING_SINGUITIES

    def frameCountBeforeGestationIsDone(self, currentFrame: int) -> int:
        return max(Spawner.GESTATION_FRAME_LAG - (currentFrame - self.lastFrameClaimed), 0)

    def tryClaimedBy(self, playerId, singuityCount, frameCount: int) -> DiscreteSpawner:
        if self.remainingSinguitiesToCapture(playerId) == 0:
            return self

        if self.remainingSinguitiesToCapture(playerId) <= singuityCount:
            return DiscreteSpawner(self.id, self.position, DiscreteSpawnerAllegence(True, playerId, Spawner.MAX_HEALTH_POINTS), frameCount)

        claimedSinguitiesAfterFrame = Spawner.REQUIRED_CAPTURING_SINGUITIES - self.remainingSinguitiesToCapture(playerId) + singuityCount

        return DiscreteSpawner(
            self.id,
            self.position,
            DiscreteSpawnerAllegence(False, playerId, Spawner.MAX_HEALTH_POINTS * claimedSinguitiesAfterFrame / Spawner.REQUIRED_CAPTURING_SINGUITIES),
            self.lastFrameClaimed
        )

    def loseHealthPointsTo(self, healthPoints: float):
        if self.allegence is None:
            return

        return DiscreteSpawner(self.id, self.position, self.allegence.loseHealthPointsTo(healthPoints), self.lastFrameClaimed)
