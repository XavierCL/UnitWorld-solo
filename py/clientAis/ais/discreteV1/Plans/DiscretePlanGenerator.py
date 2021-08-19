from typing import List, Tuple

import numpy as np

from clientAis.ais.discreteV1.models.DiscreteGameState import DiscreteGameState
from clientAis.ais.discreteV1.PhysicsEstimator import PhysicsEstimator
from clientAis.ais.discreteV1.Plans.DiscreteMove import DiscreteMove

class DiscretePlanGenerator:
    MINIMUM_MOVEMENT_DURATION = 150
    ALLOWED_NEUTRAL_CAPTURE_MOVES = None
    ALLOWED_ENEMY_SPAWNER_MOVES = None
    ALLOWED_OWN_SPAWNER_MOVES = None

    @staticmethod
    def generatePlans(gameState: DiscreteGameState, playerId: str) -> List[List[DiscreteMove]]:
        generatedMoves, shortestAllowedNeutralMovementDuration = DiscretePlanGenerator.generateNeutralSpawnerMoves(gameState, playerId)
        moves = generatedMoves

        generatedMoves, shortestAllowedEnemyMovementDuration = DiscretePlanGenerator.generateEnemySpawnerMoves(gameState, playerId)
        moves.extend(generatedMoves)

        shortestAllowedMovementDuration = min(shortestAllowedNeutralMovementDuration, shortestAllowedEnemyMovementDuration)

        moves.extend(DiscretePlanGenerator.generateOwnSpawnerMoves(gameState, playerId, shortestAllowedMovementDuration))
        moves.extend(DiscretePlanGenerator.generateClusterMoves(gameState, playerId, shortestAllowedMovementDuration))

        return [[move] for move in moves]

    @staticmethod
    def generateNeutralSpawnerMoves(gameState: DiscreteGameState, playerId: str) -> Tuple[List[DiscreteMove], int]:
        shortestAllowedMovementDuration = float('inf')
        moves = []
        movementDurations = []
        for spawner in [spawner for spawner in gameState.spawners if not spawner.isClaimed()]:
            moves.append(DiscreteMove.fromSpawner(playerId, spawner.id))
            movementDuration = PhysicsEstimator.estimateMovementDuration(gameState.playerDictionary[playerId].singuitiesMeanPosition, spawner.position, clusterStd=gameState.playerDictionary[playerId].singuitiesStd)
            movementDurations.append(movementDuration)
            shortestAllowedMovementDuration = min(shortestAllowedMovementDuration, max(movementDuration, DiscretePlanGenerator.MINIMUM_MOVEMENT_DURATION))

        sortedMoveIndices = np.argsort(movementDurations, kind="stable")
        return np.array(moves, dtype=object)[sortedMoveIndices][:DiscretePlanGenerator.ALLOWED_NEUTRAL_CAPTURE_MOVES].tolist(), shortestAllowedMovementDuration

    @staticmethod
    def generateEnemySpawnerMoves(gameState: DiscreteGameState, playerId: str) -> Tuple[List[DiscreteMove], int]:
        shortestAllowedMovementDuration = float('inf')
        moves = []
        movementDurations = []
        for spawner in [spawner for spawner in gameState.spawners if spawner.isClaimed() and not spawner.isAllegedToPlayer(playerId)]:
            moves.append(DiscreteMove.fromSpawner(playerId, spawner.id))
            movementDuration = PhysicsEstimator.estimateMovementDuration(gameState.playerDictionary[playerId].singuitiesMeanPosition, spawner.position, clusterStd=gameState.playerDictionary[playerId].singuitiesStd)
            movementDurations.append(movementDuration)
            shortestAllowedMovementDuration = min(shortestAllowedMovementDuration, max(movementDuration, DiscretePlanGenerator.MINIMUM_MOVEMENT_DURATION))

        sortedMoveIndices = np.argsort(movementDurations, kind="stable")
        return np.array(moves, dtype=object)[sortedMoveIndices][:DiscretePlanGenerator.ALLOWED_ENEMY_SPAWNER_MOVES].tolist(), shortestAllowedMovementDuration

    @staticmethod
    def generateOwnSpawnerMoves(gameState: DiscreteGameState, playerId: str, shortestAllowedMovementDuration: int) -> List[DiscreteMove]:
        moves = []
        movementDurations = []
        for spawner in [spawner for spawner in gameState.spawners if spawner.isClaimed() and spawner.isAllegedToPlayer(playerId)]:
            movementDuration = PhysicsEstimator.estimateMovementDuration(gameState.playerDictionary[playerId].singuitiesMeanPosition, spawner.position, clusterStd=gameState.playerDictionary[playerId].singuitiesStd)
            moves.append(DiscreteMove.fromSpawner(playerId, spawner.id, timeSpentAtSpawner=shortestAllowedMovementDuration))
            movementDurations.append(movementDuration)

        sortedMoveIndices = np.argsort(movementDurations, kind="stable")
        return np.array(moves, dtype=object)[sortedMoveIndices][:DiscretePlanGenerator.ALLOWED_OWN_SPAWNER_MOVES].tolist()

    @staticmethod
    def generateClusterMoves(gameState: DiscreteGameState, playerId: str, shortestAllowedMovementDuration: int) -> List[DiscreteMove]:
        moves = []
        for discretePlayer in gameState.playerDictionary.values():
            moves.append(DiscreteMove.fromPosition(playerId, discretePlayer.singuitiesMeanPosition, timeSpentAtPosition=shortestAllowedMovementDuration))

        return moves
