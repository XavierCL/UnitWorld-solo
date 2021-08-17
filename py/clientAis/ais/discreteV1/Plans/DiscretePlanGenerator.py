import math
from typing import List

from clientAis.ais.discreteV1.models.DiscreteGameState import DiscreteGameState
from clientAis.ais.discreteV1.PhysicsEstimator import PhysicsEstimator
from clientAis.ais.discreteV1.Plans.DiscreteMove import DiscreteMove

class DiscretePlanGenerator:
    MINIMUM_MOVEMENT_DURATION = 150

    @staticmethod
    def generatePlans(gameState: DiscreteGameState, playerId: str) -> List[List[DiscreteMove]]:
        shortestAllowedMovementDuration = float('inf')

        moves = []
        for spawner in [spawner for spawner in gameState.spawners if not (spawner.isClaimed() and spawner.isAllegedToPlayer(playerId))]:
            moves.append(DiscreteMove.fromSpawner(playerId, spawner.id))
            shortestAllowedMovementDuration = min(shortestAllowedMovementDuration, max(PhysicsEstimator.estimateMovementDuration(gameState.playerDictionary[playerId].singuitiesMeanPosition, spawner.position), DiscretePlanGenerator.MINIMUM_MOVEMENT_DURATION))

        shortestAllowedMovementDuration = DiscretePlanGenerator.MINIMUM_MOVEMENT_DURATION if math.isinf(shortestAllowedMovementDuration) else shortestAllowedMovementDuration
        shortestAllowedMovementDuration += 1

        for spawner in [spawner for spawner in gameState.spawners if spawner.isClaimed() and spawner.isAllegedToPlayer(playerId)]:
            moves.append(DiscreteMove.fromSpawner(playerId, spawner.id, timeSpentAtSpawner=shortestAllowedMovementDuration))

        for discretePlayer in gameState.playerDictionary.values():
            moves.append(DiscreteMove.fromPosition(playerId, discretePlayer.singuitiesMeanPosition, timeSpentAtPosition=shortestAllowedMovementDuration))

        return [[move] for move in moves]
