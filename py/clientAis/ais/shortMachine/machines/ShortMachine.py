from typing import List

from clientAis.ais.shortMachine.gameInterface.Move import Move
from clientAis.ais.shortMachine.machines.ShortMachineProperties import ShortMachineProperties
from clientAis.ais.shortMachine.machines.states.Regroup import Regroup
from clientAis.games.GameState import GameState

class ShortMachine:
    def __init__(self, gameState: GameState, currentPlayerId: str, singuityIds: List[str]):
        self.currentPlayerId = currentPlayerId
        self.singuityIds = singuityIds
        self.properties = ShortMachineProperties(len(singuityIds))
        self.currentState = Regroup(currentPlayerId).nextState(gameState, self.properties)

    def updateSinguities(self, singuityIds: List[str]):
        self.singuityIds = singuityIds
        self.properties = ShortMachineProperties(len(singuityIds))
        return self

    def nextState(self, gameState: GameState) -> Move:
        self.currentState = self.currentState.nextState(gameState, self.properties)
        return self.currentState.getMove(gameState, self.singuityIds)
