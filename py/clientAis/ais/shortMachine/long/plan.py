from __future__ import annotations

from typing import List

class Plan:
    def __init__(self, frameGenerated: int, offensiveSpawnerIds: List[str]):
        self.frameGenerated = frameGenerated
        self.offensiveSpawnerIds = offensiveSpawnerIds

    @staticmethod
    def whatIsEvenAPlan(frameGenerated: int) -> Plan:
        return Plan(frameGenerated, [])
