from typing import List

from clientAis.ais.shortMachine.gameInterface.Move import Move
from clientAis.ais.shortMachine.long.plan import Plan
from clientAis.ais.shortMachine.machines.ShortMachineProperties import ShortMachineProperties
from clientAis.ais.shortMachine.machines.states.ShortState import ShortState
from clientAis.games.GameState import GameState
from utils.arrays import first

class GoToOwnSpawner(ShortState):
    def __init__(self, properties: ShortMachineProperties, spawnerId: str):
        super().__init__(properties)
        self.spawnerId = spawnerId

    def nextState(self, gameState: GameState, plan: Plan) -> ShortState:
        return self.getDefaultState(gameState, plan)

    def getMove(self, gameState: GameState, singuityIds: List[str]) -> Move:
        spawnerPosition = first(gameState.spawners, lambda spawner: spawner.id == self.spawnerId).position
        return Move.fromPosition(singuityIds, spawnerPosition)
