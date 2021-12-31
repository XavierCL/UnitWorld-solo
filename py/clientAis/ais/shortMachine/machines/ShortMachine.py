from typing import List

from clientAis.ais.shortMachine.gameInterface.Move import Move
from clientAis.ais.shortMachine.long.plan import Plan
from clientAis.ais.shortMachine.machines.ShortMachineProperties import ShortMachineProperties
from clientAis.ais.shortMachine.machines.states.InitialState import InitialState
from clientAis.games.GameState import GameState, Singuity

class ShortMachine:
    def __init__(self, playerId: str, singuities: List[Singuity], frameCount: int):
        self.playerId = playerId
        self.singuities = singuities
        self.currentState = InitialState(ShortMachineProperties.fromSinguities(playerId, singuities, frameCount))

    def updateSinguities(self, singuities: List[Singuity], frameCount: int):
        self.singuities = singuities
        self.currentState = self.currentState.updateProperties(singuities, frameCount)
        return self

    def nextState(self, gameState: GameState, plan: Plan, clusterProperties: List[ShortMachineProperties]) -> Move:
        self.currentState = self.currentState.nextState(gameState, plan, clusterProperties)
        return self.currentState.getMove(gameState, [s.id for s in self.singuities])
