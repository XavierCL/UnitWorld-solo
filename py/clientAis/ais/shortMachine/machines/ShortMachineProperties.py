from __future__ import annotations

from typing import List

import numpy as np

from clientAis.games.GameState import Singuity

class ShortMachineProperties:
    def __init__(self, clusterId: str, playerId: str, singuityCount: int, singuityMeanPosition: np.ndarray):
        self.clusterId = clusterId
        self.playerId = playerId
        self.singuityCount = singuityCount
        self.singuityMeanPosition = singuityMeanPosition

    @staticmethod
    def fromSinguities(clusterId: str, playerId: str, singuities: List[Singuity]) -> ShortMachineProperties:
        return ShortMachineProperties(clusterId, playerId, len(singuities), np.mean([s.position for s in singuities], axis=0))

    def updateSinguities(self, singuities: List[Singuity]) -> ShortMachineProperties:
        return ShortMachineProperties.fromSinguities(self.clusterId, self.playerId, singuities)
