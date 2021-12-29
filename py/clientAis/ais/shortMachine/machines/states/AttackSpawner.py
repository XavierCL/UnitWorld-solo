from __future__ import annotations

from typing import List, Union

from clientAis.ais.shortMachine.gameInterface.Move import Move
from clientAis.ais.shortMachine.machines.ShortMachineProperties import ShortMachineProperties
from clientAis.ais.shortMachine.machines.states.ShortState import ShortState
from clientAis.games.GameState import GameState, Spawner
from utils.arrays import firstOrNone

class AttackSpawner(ShortState):
    def __init__(self, currentPlayerId: str, spawner: Spawner):
        super().__init__(currentPlayerId)
        self.spawner = spawner

    @staticmethod
    def attackIfPossible(currentPlayerId: str, spawner, clusterProperties: ShortMachineProperties) -> Union[AttackSpawner, None]:
        if (spawner.allegence is None or spawner.allegence.playerId != currentPlayerId) and clusterProperties.singuityCount >= Spawner.REQUIRED_CAPTURING_SINGUITIES:
            return AttackSpawner(currentPlayerId, spawner)
        if spawner.allegence is not None and spawner.allegence.playerId == currentPlayerId and not spawner.allegence.isClaimed and clusterProperties.singuityCount >= Spawner.REQUIRED_CAPTURING_SINGUITIES * (Spawner.MAX_HEALTH_POINTS - spawner.allegence.healthPoints) / Spawner.MAX_HEALTH_POINTS:
            return AttackSpawner(currentPlayerId, spawner)

        return None

    def nextState(self, gameState: GameState, properties: ShortMachineProperties) -> ShortState:
        self.spawner = firstOrNone(gameState.spawners, lambda spawner: spawner.id == self.spawner.id)

        if self.spawner is None:
            return self.getDefaultState(gameState, properties)

        if self.attackIfPossible(self.currentPlayerId, self.spawner, properties) is not None:
            return self

        return self.getDefaultState(gameState, properties)

    def getMove(self, gameState: GameState, singuityIds: List[str]) -> Move:
        return Move.fromSpawner(singuityIds, self.spawner, self.currentPlayerId)
