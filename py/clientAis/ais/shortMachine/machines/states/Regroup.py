from typing import List

import numpy as np

from clientAis.ais.shortMachine.gameInterface.Move import Move
from clientAis.ais.shortMachine.machines.ShortMachineProperties import ShortMachineProperties
from clientAis.ais.shortMachine.machines.states.ShortState import ShortState
from clientAis.games.GameState import GameState

class Regroup(ShortState):
    def nextState(self, gameState: GameState, properties: ShortMachineProperties) -> ShortState:
        return self.getDefaultState(gameState, properties)

    def getMove(self, gameState: GameState, singuityIds: List[str]) -> Move:
        singuityIdSet = set(singuityIds)
        meanSinguityPosition = np.median([s.position for s in gameState.singuities if s.id in singuityIdSet], axis=0)
        return Move.fromPosition(singuityIds, meanSinguityPosition)
