from typing import List, Tuple

import numpy as np

from clientAis.ais.discreteV1.models.DiscreteGameState import DiscreteGameState
from clientAis.ais.discreteV1.Plans.DiscreteMove import DiscreteMove

class DiscretePlanGenerator:
    MINIMUM_MOVEMENT_DURATION = 150
    ALLOWED_NEUTRAL_CLAIM_MOVES = 2
    ALLOWED_ENEMY_CLAIM_MOVES = 2
    ALLOWED_OWN_SPAWNER_MOVES = 1

    @staticmethod
    def executeStep(gameState: DiscreteGameState, playerId: str) -> List[Tuple[List[DiscreteMove], DiscreteGameState]]:
        neutralClaimingMoves = DiscretePlanGenerator.generateNeutralSpawnerMoves(gameState, playerId)
        neutralClaimingGameStates = [gameState.executePlan([move]) for move in neutralClaimingMoves]
        neutralClaimingGameStateFrameCounts = [g.frameCount for g in neutralClaimingGameStates]
        neutralClaimingGameStateFrameCountSortedIndices = np.argsort(neutralClaimingGameStateFrameCounts, kind="stable")
        neutralClaimingNextSteps: List[Tuple[DiscreteMove, DiscreteGameState]] = list(zip(
            np.array(neutralClaimingMoves, dtype=object)[neutralClaimingGameStateFrameCountSortedIndices][:DiscretePlanGenerator.ALLOWED_NEUTRAL_CLAIM_MOVES],
            np.array(neutralClaimingGameStates, dtype=object)[neutralClaimingGameStateFrameCountSortedIndices][:DiscretePlanGenerator.ALLOWED_NEUTRAL_CLAIM_MOVES]
        ))

        enemyClaimingMoves = DiscretePlanGenerator.generateEnemySpawnerMoves(gameState, playerId)
        enemyClaimingGameStates = [gameState.executePlan([move]) for move in enemyClaimingMoves]
        enemyClaimingGameStateFrameCounts = [g.frameCount for g in enemyClaimingGameStates]
        enemyClaimingGameStateFrameCountSortedIndices = np.argsort(enemyClaimingGameStateFrameCounts, kind="stable")
        enemyClaimingNextSteps: List[Tuple[DiscreteMove, DiscreteGameState]] = list(zip(
            np.array(enemyClaimingMoves, dtype=object)[enemyClaimingGameStateFrameCountSortedIndices][:DiscretePlanGenerator.ALLOWED_ENEMY_CLAIM_MOVES],
            np.array(enemyClaimingGameStates, dtype=object)[enemyClaimingGameStateFrameCountSortedIndices][:DiscretePlanGenerator.ALLOWED_ENEMY_CLAIM_MOVES]
        ))

        shortestFrameCounts = [step[1].frameCount for step in neutralClaimingNextSteps[:1] + enemyClaimingNextSteps[:1]]

        shortestAllowedMoveOnlyDuration = DiscretePlanGenerator.MINIMUM_MOVEMENT_DURATION if len(shortestFrameCounts) == 0 else max(DiscretePlanGenerator.MINIMUM_MOVEMENT_DURATION, min(np.array(shortestFrameCounts) - gameState.frameCount))

        nextSteps = neutralClaimingNextSteps + enemyClaimingNextSteps

        nextSteps.extend([(move, gameState.executePlan([move])) for move in DiscretePlanGenerator.generateOwnSpawnerMoves(gameState, playerId, shortestAllowedMoveOnlyDuration)])
        nextSteps.extend([(move, gameState.executePlan([move])) for move in DiscretePlanGenerator.generateClusterMoves(gameState, playerId, shortestAllowedMoveOnlyDuration)])

        return [([move], gameState) for move, gameState in nextSteps]

    @staticmethod
    def generateNeutralSpawnerMoves(gameState: DiscreteGameState, playerId: str) -> List[DiscreteMove]:
        moves = []
        for spawner in [spawner for spawner in gameState.spawners if not spawner.isClaimed()]:
            moves.append(DiscreteMove.fromSpawner(playerId, spawner.id))

        return moves

    @staticmethod
    def generateEnemySpawnerMoves(gameState: DiscreteGameState, playerId: str) -> List[DiscreteMove]:
        moves = []
        for spawner in [spawner for spawner in gameState.spawners if spawner.isClaimed() and not spawner.isAllegedToPlayer(playerId)]:
            moves.append(DiscreteMove.fromSpawner(playerId, spawner.id))

        return moves

    @staticmethod
    def generateOwnSpawnerMoves(gameState: DiscreteGameState, playerId: str, shortestAllowedMovementDuration: int) -> List[DiscreteMove]:
        moves = []
        for spawner in [spawner for spawner in gameState.spawners if spawner.isClaimed() and spawner.isAllegedToPlayer(playerId)]:
            moves.append(DiscreteMove.fromSpawner(playerId, spawner.id, timeSpentAtSpawner=shortestAllowedMovementDuration))

        return moves

    @staticmethod
    def generateClusterMoves(gameState: DiscreteGameState, playerId: str, shortestAllowedMovementDuration: int) -> List[DiscreteMove]:
        moves = []
        for discretePlayer in gameState.playerDictionary.values():
            moves.append(DiscreteMove.fromPosition(playerId, discretePlayer.singuitiesMeanPosition, timeSpentAtPosition=shortestAllowedMovementDuration))

        return moves
