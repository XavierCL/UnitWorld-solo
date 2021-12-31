from typing import Union

import numpy as np

from clientAis.ais.shortMachine.long.plan import Plan
from clientAis.games.GameState import GameState
from utils.math import distance2

class PlanProvider:
    def __init__(self, maximumFrameBeforeRecompute=300):
        self.lastPlan: Union[Plan, None] = None
        self.maximumFrameBeforeRecompute = maximumFrameBeforeRecompute

    def getPlan(self, gameState: GameState, playerId: str) -> Plan:
        if self.lastPlan is None or gameState.frameCount - self.lastPlan.frameGenerated > self.maximumFrameBeforeRecompute:
            self.lastPlan = self.computePlan(gameState, playerId)

        return self.lastPlan

    def computePlan(self, gameState: GameState, playerId: str) -> Plan:
        selfClaimedSpawners = [s for s in gameState.spawners if s.allegence is not None and s.allegence.playerId == playerId and s.allegence.isClaimed]
        ownSinguityPositions = [s.position for s in gameState.singuities if s.playerId == playerId]
        attackableSpawners = [s for s in gameState.spawners if s.allegence is None or s.allegence.playerId != playerId or not s.allegence.isClaimed]

        if len(selfClaimedSpawners) == 0 or len(attackableSpawners) == 0:
            return Plan.whatIsEvenAPlan(gameState.frameCount)

        ownMeanSpawnerPosition = np.mean([s.position for s in selfClaimedSpawners], axis=0)
        ownMeanSinguityPosition = 0 if len(ownSinguityPositions) == 0 else np.mean(ownSinguityPositions, axis=0)
        ownMeanPosition = ownMeanSpawnerPosition * 0.7 + ownMeanSinguityPosition * 0.3

        attackableSpawnerDistanceToOwnMean = distance2(np.array([s.position for s in attackableSpawners]), ownMeanPosition)
        sortedAttackableSpawnerIndices = np.argsort(attackableSpawnerDistanceToOwnMean)

        return Plan(gameState.frameCount, [s.id for s in np.array(attackableSpawners, dtype=object)[sortedAttackableSpawnerIndices]])
