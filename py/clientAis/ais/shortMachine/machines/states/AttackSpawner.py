from __future__ import annotations

from typing import List, Union

import numpy as np

from clientAis.ais.shortMachine.gameInterface.Move import Move
from clientAis.ais.shortMachine.long.plan import Plan
from clientAis.ais.shortMachine.machines.ShortMachineProperties import ShortMachineProperties
from clientAis.ais.shortMachine.machines.states.ShortState import ShortState
from clientAis.games.GameState import GameState, Spawner
from utils.arrays import firstOrNone

class AttackSpawner(ShortState):
    def __init__(self, properties: ShortMachineProperties, spawner: Spawner):
        super().__init__(properties)
        self.spawner = spawner

    @staticmethod
    def attackIfWorthIt(clusterProperties: ShortMachineProperties, spawner, properties: List[ShortMachineProperties]) -> Union[AttackSpawner, None]:
        if spawner.allegence is None or spawner.allegence.playerId == clusterProperties.playerId:
            # Cannot attack neutral or own spawner
            return None

        enemyClusters = [p for p in properties if p.playerId != clusterProperties.playerId]

        # No enemies, attack indeed
        if len(enemyClusters) == 0:
            return AttackSpawner(clusterProperties, spawner)

        enemyClusterDistances = np.sum((np.array([p.singuityMeanPosition for p in enemyClusters]) - spawner.position) ** 2, axis=1)
        closestEnemyClusterIndex = np.argmin(enemyClusterDistances)

        # Closest enemy is too far, ignore it
        if enemyClusterDistances[closestEnemyClusterIndex] > 400 ** 2:
            return AttackSpawner(clusterProperties, spawner)

        # current cluster is more numerous than enemy defender, attack indeed.
        if enemyClusters[closestEnemyClusterIndex].singuityCount * 4 < clusterProperties.singuityCount:
            return AttackSpawner(clusterProperties, spawner)

        # Not enough units to attack, don't attack
        return None

    def canStillAttack(self) -> bool:
        if self.spawner.allegence is None or self.spawner.allegence.playerId == self.properties.playerId:
            # Cannot attack neutral or own spawner
            return False

        return True

    def nextState(self, gameState: GameState, plan: Plan, properties: List[ShortMachineProperties]) -> ShortState:
        self.spawner = firstOrNone(gameState.spawners, lambda spawner: spawner.id == self.spawner.id)

        if self.spawner is None:
            return self.getDefaultState(gameState, plan, properties)

        if self.canStillAttack():
            return self

        from clientAis.ais.shortMachine.machines.states.CaptureSpawner import CaptureSpawner

        captureSpawner = CaptureSpawner.captureIfPossible(self.properties, self.spawner)

        if captureSpawner is not None:
            return captureSpawner

        return self.getDefaultState(gameState, plan, properties)

    def getMove(self, gameState: GameState, singuityIds: List[str]) -> Move:
        return Move.fromPosition(singuityIds, self.spawner.position)
