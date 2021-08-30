from typing import List, Optional, Tuple

import numpy as np

from clientAis.ais.discreteV1.models.DiscreteGameState import DiscreteGameState
from clientAis.ais.discreteV1.Plans.DiscreteMove import DiscreteMove

class DiscreteMoveGenerator:
    MINIMUM_MOVEMENT_DURATION = 150
    ALLOWED_NEUTRAL_CLAIM_MOVES = 1
    ALLOWED_ENEMY_CLAIM_MOVES = 2
    ALLOWED_OWN_SPAWNER_MOVES = 1

    @staticmethod
    def executeStep(gameState: DiscreteGameState, playerId: str) -> List[Tuple[DiscreteMove, DiscreteGameState]]:
        neutralClaimingNextSteps = DiscreteMoveGenerator.getQuickestFirstNSteps(gameState, DiscreteMoveGenerator.generateNeutralSpawnerMoves(gameState, playerId), DiscreteMoveGenerator.ALLOWED_NEUTRAL_CLAIM_MOVES)
        enemyClaimingNextSteps = DiscreteMoveGenerator.getQuickestFirstNSteps(gameState, DiscreteMoveGenerator.generateEnemySpawnerMoves(gameState, playerId), DiscreteMoveGenerator.ALLOWED_ENEMY_CLAIM_MOVES)

        shortestFrameCounts = [step[1].frameCount for step in neutralClaimingNextSteps[:1] + enemyClaimingNextSteps[:1]]

        shortestAllowedMoveOnlyDuration = DiscreteMoveGenerator.MINIMUM_MOVEMENT_DURATION if len(shortestFrameCounts) == 0 else max(
            DiscreteMoveGenerator.MINIMUM_MOVEMENT_DURATION, min(np.array(shortestFrameCounts) - gameState.frameCount)
        )

        nextSteps = neutralClaimingNextSteps + enemyClaimingNextSteps

        nextSteps.extend(DiscreteMoveGenerator.getQuickestFirstNSteps(gameState, DiscreteMoveGenerator.generateOwnSpawnerMoves(gameState, playerId, shortestAllowedMoveOnlyDuration), DiscreteMoveGenerator.ALLOWED_OWN_SPAWNER_MOVES))
        nextSteps.extend([(move, gameState.executeMove(move)) for move in DiscreteMoveGenerator.generateClusterMoves(gameState, playerId, shortestAllowedMoveOnlyDuration)])

        return nextSteps

    @staticmethod
    def getQuickestFirstNSteps(originalGameState: DiscreteGameState, moves: List[DiscreteMove], limit: Optional[int] = None) -> List[Tuple[DiscreteMove, DiscreteGameState]]:
        durationsAndPartialStates = [originalGameState.preprocessMove(move) for move in moves]
        durations = [duration for duration, _ in durationsAndPartialStates]
        durationSortedIndices = np.argsort(durations, kind="stable")
        return list(
            zip(
                np.array(moves, dtype=object)[durationSortedIndices][:limit],
                [partialGameState() for _, partialGameState in np.array(durationsAndPartialStates, dtype=object)[durationSortedIndices][:limit]]
            )
        )

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
    def generateClusterMoves(gameState: DiscreteGameState, playerId: str, shortestAllowedMovementDuration: int, ownOnly=False) -> List[DiscreteMove]:
        moves = []
        for discretePlayer in [p for p in gameState.playerDictionary.values() if p.id == playerId or not ownOnly]:
            moves.append(DiscreteMove.fromPosition(playerId, discretePlayer.singuitiesMeanPosition, timeSpentAtPosition=shortestAllowedMovementDuration))

        return moves
