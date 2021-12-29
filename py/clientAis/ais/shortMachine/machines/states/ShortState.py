from __future__ import annotations

from typing import List

from clientAis.ais.shortMachine.gameInterface.Move import Move
from clientAis.ais.shortMachine.machines.ShortMachineProperties import ShortMachineProperties
from clientAis.games.GameState import GameState

class ShortState:
    def __init__(self, currentPlayerId: str):
        self.currentPlayerId = currentPlayerId

    def nextState(self, gameState: GameState, properties: ShortMachineProperties) -> ShortState:
        raise NotImplementedError

    def getMove(self, gameState: GameState, singuityIds: List[str]) -> Move:
        raise NotImplementedError

    def getDefaultState(self, gameState: GameState, properties: ShortMachineProperties) -> ShortState:
        # todo
        # Attack closest spawner
        # Consider merge
        # Consider regroup to closest spawner
        from clientAis.ais.shortMachine.machines.states.AttackSpawner import AttackSpawner

        for spawner in gameState.spawners:
            newState = AttackSpawner.attackIfPossible(self.currentPlayerId, spawner, properties)
            if newState is not None:
                return newState

        from clientAis.ais.shortMachine.machines.states.Regroup import Regroup
        return Regroup(self.currentPlayerId)
