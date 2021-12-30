from clientAis.ais.Artificial import Artificial
from clientAis.ais.shortMachine.machines.ShortMachineManager import ShortMachineManager
from clientAis.communications.ServerCommander import ServerCommander
from clientAis.games.GameState import GameState

class ShortMachineAi(Artificial):
    def __init__(self, serverCommander: ServerCommander):
        super().__init__(serverCommander)
        self.shortMachineManager = ShortMachineManager()

    def frame(self, gameState: GameState, currentPlayerId: str):
        moves = self.shortMachineManager.fromGameState(gameState, currentPlayerId)

        for move in moves:
            if move.singuityIds is None:
                continue
            elif move.targetsPosition:
                self.serverCommander.moveUnitsToPosition(move.singuityIds, move.position)
            else:
                self.serverCommander.moveUnitsToSpawner(move.singuityIds, move.spawnerId)
