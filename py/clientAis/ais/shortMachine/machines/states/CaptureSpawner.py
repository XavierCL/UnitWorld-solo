from __future__ import annotations

from typing import List, Union

from clientAis.ais.shortMachine.gameInterface.Move import Move
from clientAis.ais.shortMachine.long.plan import Plan
from clientAis.ais.shortMachine.machines.ShortMachineProperties import ShortMachineProperties
from clientAis.ais.shortMachine.machines.states.ShortState import ShortState
from clientAis.games.GameState import GameState, Spawner
from utils.arrays import firstOrNone

class CaptureSpawner(ShortState):
    def __init__(self, properties: ShortMachineProperties, spawner: Spawner):
        super().__init__(properties)
        self.spawner = spawner

    @staticmethod
    def captureIfPossible(clusterProperties: ShortMachineProperties, spawner) -> Union[CaptureSpawner, None]:
        if spawner.allegence is None:
            if clusterProperties.singuityCount >= Spawner.REQUIRED_CAPTURING_SINGUITIES:
                return CaptureSpawner(clusterProperties, spawner)
            return None

        if spawner.allegence.isClaimed or spawner.allegence.playerId != clusterProperties.playerId:
            # Cant capture an already captured spawner or a spawner an enemy started capturing.
            # Either move to it or attack it
            return None

        if clusterProperties.singuityCount >= Spawner.REQUIRED_CAPTURING_SINGUITIES * (Spawner.MAX_HEALTH_POINTS - spawner.allegence.healthPoints) / Spawner.MAX_HEALTH_POINTS:
            return CaptureSpawner(clusterProperties, spawner)

        # Not enough units to capture, don't even try
        return None

    def nextState(self, gameState: GameState, plan: Plan) -> ShortState:
        self.spawner = firstOrNone(gameState.spawners, lambda spawner: spawner.id == self.spawner.id)

        if self.spawner is None:
            return self.getDefaultState(gameState, plan)

        if self.captureIfPossible(self.properties, self.spawner) is not None:
            return self

        return self.getDefaultState(gameState, plan)

    def getMove(self, gameState: GameState, singuityIds: List[str]) -> Move:
        return Move.fromSpawner(singuityIds, self.spawner, self.properties.playerId)
