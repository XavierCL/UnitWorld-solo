from __future__ import annotations

from typing import List, Union

import numpy as np

from clientAis.ais.shortMachine.gameInterface.Move import Move
from clientAis.ais.shortMachine.long.plan import Plan
from clientAis.ais.shortMachine.machines.ShortMachineProperties import ShortMachineProperties
from clientAis.ais.shortMachine.machines.states.ShortState import ShortState
from clientAis.games.GameState import GameState, Singuity, Spawner
from utils.arrays import firstOrNone
from utils.math import distance, distance2

class AttackSpawner(ShortState):
    def __init__(self, properties: ShortMachineProperties, spawner: Spawner):
        super().__init__(properties)
        self.spawner = spawner

    def nextState(self, gameState: GameState, plan: Plan, properties: List[ShortMachineProperties]) -> ShortState:
        self.spawner = firstOrNone(gameState.spawners, lambda spawner: spawner.id == self.spawner.id)

        if self.spawner is None:
            return self.getDefaultState(gameState, plan, properties)

        if self.canStillAttack(properties):
            return self

        return self.getDefaultState(gameState, plan, properties)

    def getMove(self, gameState: GameState, singuityIds: List[str]) -> Move:
        return Move.fromPosition(singuityIds, self.spawner.position)

    @staticmethod
    def attackIfWorthIt(clusterProperties: ShortMachineProperties, spawner: Spawner, properties: List[ShortMachineProperties]) -> Union[AttackSpawner, None]:
        if AttackSpawner.isWorthAttacking(clusterProperties, spawner, properties, 1.9, 100):
            return AttackSpawner(clusterProperties, spawner)

        # Not enough units to attack, don't attack
        return None

    def canStillAttack(self, allProperties: List[ShortMachineProperties]) -> bool:
        return AttackSpawner.isWorthAttacking(self.properties, self.spawner, allProperties, 1.1, 50)

    @staticmethod
    def isWorthAttacking(
        clusterProperties: ShortMachineProperties,
        spawner: Spawner,
        properties: List[ShortMachineProperties],
        singuityRatio: float,
        spawnerInfluence: float
    ) -> bool:
        if spawner.allegence is None or spawner.allegence.playerId == clusterProperties.playerId:
            # Cannot attack neutral or own spawner
            return False

        enemyCountFromDistance = 0

        if spawner.allegence.isClaimed:
            frameUntilArrival = distance(clusterProperties.singuityMeanPosition, spawner.position) / Singuity.MAXIMUM_SPEED_UNITS_PER_FRAME
            remainingGestationFrame = max(Spawner.GESTATION_FRAME_LAG - (clusterProperties.frameCount - spawner.lastClaimFrameCount), 0)
            activeSpawningFrames = max(frameUntilArrival - remainingGestationFrame, 0)
            enemyCountFromDistance = activeSpawningFrames / Spawner.SPAWN_FRAME_LAG

        enemyClusters = [p for p in properties if p.playerId != clusterProperties.playerId]
        totalEnemiesAtLocation = enemyCountFromDistance

        # No enemies, attack indeed
        if len(enemyClusters) > 0:
            enemyClusterDistances = distance2(np.array([p.singuityMeanPosition for p in enemyClusters]), spawner.position)
            closestEnemyClusterIndex = np.argmin(enemyClusterDistances)

            # Closest enemy is too far, ignore it
            if enemyClusterDistances[closestEnemyClusterIndex] < 400 ** 2:
                totalEnemiesAtLocation += enemyClusters[closestEnemyClusterIndex].singuityCount

        # current cluster is more numerous than enemy defender, attack indeed.
        if totalEnemiesAtLocation * singuityRatio + spawnerInfluence * spawner.allegence.healthPoints / Spawner.MAX_HEALTH_POINTS < clusterProperties.singuityCount:
            return True

        # Not enough units to attack, don't attack
        return False
