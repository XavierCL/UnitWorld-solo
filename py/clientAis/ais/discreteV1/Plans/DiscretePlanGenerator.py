from typing import List

from clientAis.ais.discreteV1.models.DiscreteGameState import DiscreteGameState
from clientAis.ais.discreteV1.Plans.DiscreteMove import DiscreteMove

class DiscretePlanGenerator:
    @staticmethod
    def generatePlans(gameState: DiscreteGameState, playerId: str) -> List[List[DiscreteMove]]:
        shortestAllowedMovementDuration = 300

        moves = []
        for spawner in [spawner for spawner in gameState.spawners if not (spawner.isClaimed() and spawner.isAllegedToPlayer(playerId))]:
            moves.append(DiscreteMove.fromSpawner(playerId, spawner.id))

        for spawner in [spawner for spawner in gameState.spawners if spawner.isClaimed() and spawner.isAllegedToPlayer(playerId)]:
            moves.append(DiscreteMove.fromSpawner(playerId, spawner.id, timeSpentAtSpawner=shortestAllowedMovementDuration))

        for discretePlayer in gameState.playerDictionary.values():
            moves.append(DiscreteMove.fromPosition(playerId, discretePlayer.singuitiesMeanPosition, timeSpentAtPosition=shortestAllowedMovementDuration))

        return [[move] for move in moves]
