from typing import List

import numpy as np

from clientAis.ais.discreteV1.models.DiscreteGameState import DiscreteGameState
from clientAis.ais.discreteV1.PhysicsEstimator import PhysicsEstimator
from clientAis.ais.discreteV1.Plans.DiscreteMove import DiscreteMove

class DiscretePlanGenerator:
    def generatePlans(self, gameState: DiscreteGameState, playerId: str) -> List[List[DiscreteMove]]:
        movementDurations = []

        moves = []
        for spawner in gameState.spawners:
            moves.append(DiscreteMove.fromSpawner(playerId, spawner.id))
            movementDurations.append(PhysicsEstimator.estimateMovementDuration(gameState.playerDictionary[playerId].singuitiesMeanPosition, spawner.position))

        shortestAllowedMovementDuration = np.sort(movementDurations)[1]

        for _, discretePlayer in gameState.playerDictionary.items():
            moves.append(DiscreteMove.fromPosition(playerId, discretePlayer.singuitiesMeanPosition, timeSpentAtPosition=shortestAllowedMovementDuration))

        return [[move] for move in moves]
