from __future__ import annotations

from typing import List

import numpy as np

from clientAis.ais.shortMachine.gameInterface.Move import Move
from clientAis.ais.shortMachine.long.plan import Plan
from clientAis.ais.shortMachine.machines.ShortMachineProperties import ShortMachineProperties
from clientAis.games.GameState import GameState, Singuity

class ShortState:
    def __init__(self, properties: ShortMachineProperties):
        self.properties = properties

    def updateProperties(self, singuities: List[Singuity]) -> ShortState:
        self.properties = self.properties.updateSinguities(singuities)
        return self

    def nextState(self, gameState: GameState, plan: Plan, properties: List[ShortMachineProperties]) -> ShortState:
        raise NotImplementedError

    def getMove(self, gameState: GameState, singuityIds: List[str]) -> Move:
        raise NotImplementedError

    def getDefaultState(self, gameState: GameState, plan: Plan, properties: List[ShortMachineProperties]) -> ShortState:
        from clientAis.ais.shortMachine.machines.states.CaptureSpawner import CaptureSpawner
        from clientAis.ais.shortMachine.machines.states.AttackSpawner import AttackSpawner

        spawnerById = {s.id: s for s in gameState.spawners}

        for spawnerId in plan.offensiveSpawnerIds:
            spawnerStateGenerator = [
                lambda: CaptureSpawner.captureIfPossible(self.properties, spawnerById[spawnerId]),
                lambda: AttackSpawner.attackIfWorthIt(self.properties, spawnerById[spawnerId], properties)
            ]

            for stateGenerator in spawnerStateGenerator:
                newState = stateGenerator()

                if newState is not None:
                    return newState

        from clientAis.ais.shortMachine.machines.states.GoToOwnSpawner import GoToOwnSpawner

        ownSpawners = [s for s in gameState.spawners if s.allegence is not None and s.allegence.playerId == self.properties.playerId and s.allegence.isClaimed]
        closestSpawner = np.argmin(np.sum((np.array([s.position for s in ownSpawners]) - self.properties.singuityMeanPosition) ** 2, axis=1))

        return GoToOwnSpawner(self.properties, ownSpawners[closestSpawner].id)
