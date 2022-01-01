from typing import List

from clientAis.ais.shortMachine.gameInterface.Move import Move
from clientAis.ais.shortMachine.long.plan import Plan
from clientAis.ais.shortMachine.machines.states.ShortState import ShortState
from clientAis.games.GameState import GameState

class InitialState(ShortState):
    def nextState(self, gameState: GameState, plan: Plan) -> ShortState:
        return self.getDefaultState(gameState, plan)

    def getMove(self, gameState: GameState, singuityIds: List[str]) -> Move:
        raise Exception("Cannot ask move from the initial state. This can only be called a next state.")
