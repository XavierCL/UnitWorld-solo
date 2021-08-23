from typing import List

from clientAis.ais.Artificial import Artificial
from clientAis.ais.discreteV1.DiscreteGameScorer import DiscreteGameScorer
from clientAis.ais.discreteV1.DiscreteGameSearcher import DiscreteGameSearcher
from clientAis.ais.discreteV1.models.DiscreteGameState import DiscreteGameState
from clientAis.ais.discreteV1.Plans.DiscreteMoveGenerator import DiscreteMoveGenerator
from clientAis.ais.discreteV1.Move import Move
from clientAis.games.GameState import GameState
from clientAis.communications.ServerCommander import ServerCommander

class DiscreteV1Ai(Artificial):
    def __init__(self, serverCommander: ServerCommander):
        super().__init__(serverCommander)

        self.currentPlayerId: str = None
        self.gameSearcher = DiscreteGameSearcher(allottedGenerationTimeSeconds=0.3)

    def frame(self, gameState: GameState, currentPlayerId: str):
        self.currentPlayerId = currentPlayerId

        if self.frameIsIgnored(gameState):
            return

        currentGameState = DiscreteGameState.fromGameState(gameState, currentPlayerId)
        plan: List[Move] = self.gameSearcher.getBestOwnPlan(currentGameState)

        for move in plan:
            if move.singuityIds is None:
                continue
            elif move.targetsPosition:
                self.serverCommander.moveUnitsToPosition(move.singuityIds, move.position)
            else:
                self.serverCommander.moveUnitsToSpawner(move.singuityIds, move.spawnerId)

    def frameIsIgnored(self, gameState: GameState) -> bool:
        notOwnSpawners = [s for s in gameState.spawners if s.allegence is None or not s.allegence.isClaimed or (s.allegence.isClaimed and s.allegence.playerId != self.currentPlayerId)]

        if len(notOwnSpawners) == 0:
            # Game is won
            return True

        ownSpawners = [s for s in gameState.spawners if s.allegence is not None and s.allegence.isClaimed and s.allegence.playerId == self.currentPlayerId]

        if len(ownSpawners) == 0:
            # Game is lost
            return True

        ownSinguities = [s for s in gameState.singuities if s.playerId == self.currentPlayerId]

        if len(ownSinguities) == 0:
            # No units to control
            return True

        return False
