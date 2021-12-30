from typing import List

import numpy as np

from clientAis.ais.Artificial import Artificial
from clientAis.ais.discreteV1.models.DiscreteGameState import DiscreteGameState
from clientAis.ais.discreteV1.Plans.Move import Move
from clientAis.ais.discreteV1.searches.DiscreteGameSearcher import DiscreteGameSearcher
from clientAis.communications.ServerCommander import ServerCommander
from clientAis.games.GameState import GameState

np.seterr(all='raise')

class DiscreteV1Ai(Artificial):
    def __init__(self, serverCommander: ServerCommander):
        super().__init__(serverCommander)

        self.currentPlayerId: str = None
        self.gameSearcher = DiscreteGameSearcher(maxDepth=1)

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

    def frameTimeSecond(self) -> float:
        if self.gameSearcher.allottedGenerationTimeSeconds is None:
            return super().frameTimeSecond()

        return self.gameSearcher.allottedGenerationTimeSeconds

    def frameIsIgnored(self, gameState: GameState) -> bool:
        if len(gameState.players) <= 1:
            # Waiting for other players
            return True

        notOwnSpawners = [s for s in gameState.spawners if
                          s.allegence is None or not s.allegence.isClaimed or (s.allegence.isClaimed and s.allegence.playerId != self.currentPlayerId)]

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
