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

    def nextState(self, gameState: GameState, plan: Plan) -> ShortState:
        self.spawner = firstOrNone(gameState.spawners, lambda spawner: spawner.id == self.spawner.id)

        if self.spawner is None:
            return self.getDefaultState(gameState, plan)

        if self.canStillAttack(gameState):
            return self

        return self.getDefaultState(gameState, plan)

    def getMove(self, gameState: GameState, singuityIds: List[str]) -> Move:
        return Move.fromPosition(singuityIds, self.spawner.position)

    @staticmethod
    def attackIfWorthIt(clusterProperties: ShortMachineProperties, spawner: Spawner, gameState: GameState) -> Union[AttackSpawner, None]:
        if AttackSpawner.isWorthAttacking(clusterProperties, spawner, gameState, 3.5, 1.0):
            return AttackSpawner(clusterProperties, spawner)

        # Not enough units to attack, don't attack
        return None

    def canStillAttack(self, gameState: GameState) -> bool:
        return AttackSpawner.isWorthAttacking(self.properties, self.spawner, gameState, 3.1, 0.9)

    @staticmethod
    def isWorthAttacking(
        clusterProperties: ShortMachineProperties,
        spawner: Spawner,
        gameState: GameState,
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

        enemySinguityPositions = [s.position for s in gameState.singuities if s.playerId != clusterProperties.playerId]
        closeEnemyCount = 0 if len(enemySinguityPositions) == 0 else np.count_nonzero(distance2(np.array(enemySinguityPositions), spawner.position) < 500 ** 2)
        totalEnemiesAtLocation = enemyCountFromDistance + closeEnemyCount

        # current cluster is more numerous than enemy defender, attack indeed.
        spawnerDiesFirst = totalEnemiesAtLocation + spawnerInfluence * spawner.allegence.healthPoints / Singuity.MAX_HEALTH_POINT
        defendersDieFirst = totalEnemiesAtLocation * singuityRatio
        enemyDefense = min(spawnerDiesFirst, defendersDieFirst)

        if enemyDefense < clusterProperties.singuityCount:
            return True

        # Not enough units to attack, don't attack
        return False
