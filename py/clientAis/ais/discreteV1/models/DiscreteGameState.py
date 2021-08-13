from __future__ import annotations

import math
from typing import Dict, List, Optional, Tuple

import numpy as np

from clientAis.ais.discreteV1.Move import Move
from clientAis.ais.discreteV1.PhysicsEstimator import PhysicsEstimator
from clientAis.ais.discreteV1.Plans.DiscreteMove import DiscreteMove
from clientAis.games.GameState import GameState, Singuity, Spawner, SpawnerAllegence
from utils import arrays

class DiscretePlayer:
    def __init__(self, id: str, singuityCount: int, singuitiesMeanPosition: np.ndarray, singuitiesStd: float):
        self.id = id
        self.singuityCount = singuityCount
        self.singuitiesMeanPosition = singuitiesMeanPosition
        self.singuitiesStd = singuitiesStd

    @staticmethod
    def fromGameState(gameState: GameState, playerId: str) -> DiscretePlayer:
        playerSinguities = [s for s in gameState.singuities if s.playerId == playerId]
        singuityPositions = [s.position for s in playerSinguities]
        return DiscretePlayer(playerId, len(playerSinguities), np.median(singuityPositions, axis=0), np.linalg.norm(np.std(singuityPositions, axis=0)))

    def executeMove(self, spawnersById: Dict[str, DiscreteSpawner], move: DiscreteMove) -> Tuple[int, DiscretePlayer]:
        targetPosition = move.position if move.position is not None else spawnersById[move.spawnerId].position
        movementDuration = PhysicsEstimator.estimateMovementDuration(self.singuitiesMeanPosition, targetPosition)

        if move.spawnerId is None:
            return movementDuration, DiscretePlayer(self.id, self.singuityCount, targetPosition, self.singuitiesStd)
        else:
            spawner = spawnersById[move.spawnerId]
            if spawner.isClaimed() and spawner.isAllegedToPlayer(move.playerId):
                return movementDuration, DiscretePlayer(self.id, self.singuityCount, targetPosition, self.singuitiesStd)
            else:
                return (
                    movementDuration + PhysicsEstimator.estimateSpawnerToZeroHealthDuration(self.singuityCount, spawner.getHealthPoints()),
                    DiscretePlayer(self.id, max(self.singuityCount - spawner.remainingSinguitiesToCapture(self.id), 0), spawner.position, self.singuitiesStd)
                )

    def appendNewSpawned(self, spawner, anteMoveFrameCount: int, postMoveFrameCount: int) -> DiscretePlayer:
        matureSpawnerDuration = min(postMoveFrameCount - anteMoveFrameCount, postMoveFrameCount - spawner.lastFrameClaimed + Spawner.GESTATION_FRAME_LAG)
        durationToTarget = PhysicsEstimator.estimateMovementDuration(spawner.position, self.singuitiesMeanPosition)
        atTargetCount = max((matureSpawnerDuration - durationToTarget) * PhysicsEstimator.SPAWNER_SPAWN_PER_FRAME, 0)
        furthestSinguity = (self.singuitiesMeanPosition - spawner.position) matureSpawnerDuration *

        return DiscretePlayer(self.id, self.singuityCount + matureSpawnerDuration * PhysicsEstimator.SPAWNER_SPAWN_PER_FRAME, )

class DiscreteSpawnerAllegence:
    def __init__(self, isClaimed: bool, playerId: str, healthPoints: float):
        self.isClaimed = isClaimed
        self.playerId = playerId
        self.healthPoints = healthPoints

    @staticmethod
    def fromAllegence(allegence: Optional[SpawnerAllegence]) -> Optional[DiscreteSpawnerAllegence]:
        return None if allegence is None else DiscreteSpawnerAllegence(allegence.isClaimed, allegence.playerId, allegence.healthPoints)

class DiscreteSpawner:
    def __init__(self, id: str, position: np.ndarray, allegence: Optional[DiscreteSpawnerAllegence], lastFrameClaimed: int):
        self.id = id
        self.position = position
        self.allegence = allegence
        self.lastFrameClaimed = lastFrameClaimed

    @staticmethod
    def fromSpawner(spawner: Spawner) -> DiscreteSpawner:
        return DiscreteSpawner(spawner.id, spawner.position, DiscreteSpawnerAllegence.fromAllegence(spawner.allegence), spawner.lastClaimFrameCount)

    def isClaimed(self):
        return self.allegence is not None and self.allegence.isClaimed

    def isAllegedToPlayer(self, playerId):
        return self.allegence is not None and self.allegence.playerId == playerId

    def getHealthPoints(self):
        return 0 if self.allegence is None else self.allegence.healthPoints

    def remainingSinguitiesToCapture(self, playerId: str) -> int:
        if self.allegence is None:
            return Spawner.REQUIRED_CAPTURING_SINGUITIES
        elif self.allegence.playerId == playerId:
            if self.allegence.isClaimed:
                return 0
            else:
                return int(math.ceil(Spawner.REQUIRED_CAPTURING_SINGUITIES - Spawner.REQUIRED_CAPTURING_SINGUITIES * self.allegence.healthPoints / Spawner.MAX_HEALTH_POINTS))
        else:
            return Spawner.REQUIRED_CAPTURING_SINGUITIES

    def frameCountBeforeGestationIsDone(self, currentFrame: int) -> int:
        return max(Spawner.GESTATION_FRAME_LAG - (currentFrame - self.lastFrameClaimed), 0)

    def executeMove(self, move: DiscreteMove, player: DiscretePlayer, postMoveFrameCount: int) -> DiscreteSpawner:
        if move.spawnerId is not None and move.spawnerId == self.id:
            remainingSinguities = self.remainingSinguitiesToCapture(player.id)
            if player.singuityCount >= remainingSinguities:
                return DiscreteSpawner(self.id, self.position, DiscreteSpawnerAllegence(True, player.id, Spawner.MAX_HEALTH_POINTS), postMoveFrameCount)
            else:
                return DiscreteSpawner(
                    self.id,
                    self.position,
                    DiscreteSpawnerAllegence(
                        False,
                        player.id,
                        Spawner.MAX_HEALTH_POINTS * (Spawner.REQUIRED_CAPTURING_SINGUITIES - remainingSinguities + player.singuityCount) / Spawner.REQUIRED_CAPTURING_SINGUITIES
                    ),
                    self.lastFrameClaimed
                )

        return self

class DiscreteGameState:
    def __init__(
        self,
        currentPlayerId: str,
        playerDictionary: Dict[str, DiscretePlayer],
        playerIds: List[str],
        currentPlayerIndex: int,
        spawners: List[DiscreteSpawner],
        spawnersById: Dict[str, DiscreteSpawner],
        frameCount: int,
        gameState: Optional[GameState]
    ):
        self.currentPlayerId = currentPlayerId
        self.playerDictionary = playerDictionary
        self.playerIds = playerIds
        self.currentPlayerIndex = currentPlayerIndex
        self.spawners = spawners
        self.spawnersById = spawnersById
        self.frameCount = frameCount
        self.rootGameState = gameState

    def hasWon(self, playerId):
        return len([s for s in self.spawners if not (s.isClaimed() and s.isAllegedToPlayer(playerId))]) == 0

    @staticmethod
    def fromGameState(gameState: GameState, currentPlayerId: str) -> DiscreteGameState:
        playerCount = len(gameState.players)
        playerDictionary: Dict[str, DiscretePlayer] = {}
        playerIds: List[str] = [None for _ in range(playerCount)]
        currentPlayerIndex = None

        for playerIndex, player in enumerate(gameState.players):
            playerDictionary[player.id] = DiscretePlayer.fromGameState(gameState, player.id)
            playerIds[playerIndex] = player.id

            if player.id == currentPlayerId:
                currentPlayerIndex = playerIndex

        spawners = [DiscreteSpawner.fromSpawner(s) for s in gameState.spawners]
        spawnersById: Dict[str, DiscreteSpawner] = {s.id: s for s in spawners}

        return DiscreteGameState(currentPlayerId, playerDictionary, playerIds, currentPlayerIndex, spawners, spawnersById, gameState.frameCount, gameState)

    def executePlan(self, plan: List[DiscreteMove]) -> DiscreteGameState:
        newPlayersById: Dict[str, DiscretePlayer] = {}
        newSpawnersById: Dict[str, DiscreteSpawner] = {}

        def getLastPlayerVersion(playerId: str) -> DiscretePlayer:
            if playerId in newPlayersById:
                return newPlayersById[playerId]

            return self.playerDictionary[playerId]

        def getLastSpawnerVersion(spawnerId: str) -> DiscreteSpawner:
            if spawnerId in newSpawnersById:
                return newSpawnersById[spawnerId]

            return self.spawnersById[spawnerId]

        moveDuration = 0

        for move in plan:
            oldPlayer = getLastPlayerVersion(move.playerId)
            moveDuration, newPlayersById[move.playerId] = oldPlayer.executeMove(self.spawnersById, move)

            if move.spawnerId is not None:
                oldSpawner = getLastSpawnerVersion(move.spawnerId)
                newSpawnersById[move.spawnerId] = oldSpawner.executeMove(move, oldPlayer, self.frameCount + moveDuration)

        # Assign new spawned singuities
        for spawner in self.spawners:
            if spawner.isClaimed():
                newPlayersById[spawner.allegence.playerId] = newPlayersById[spawner.allegence.playerId].appendNewSpawned(spawner)

        return DiscreteGameState(
            self.currentPlayerId,
            {playerId: getLastPlayerVersion(playerId) for playerId in self.playerIds},
            self.playerIds,
            self.currentPlayerIndex,
            [getLastSpawnerVersion(spawner.id) for spawner in self.spawners],
            {spawner.id: getLastSpawnerVersion(spawner.id) for spawner in self.spawners},
            self.frameCount + moveDuration,
            None
        )

    def discreteMoveToMove(self, discreteMove: DiscreteMove) -> Move:
        singuityIds = [s.id for s in self.rootGameState.singuities if s.playerId == discreteMove.playerId]

        if discreteMove.spawnerId is not None:
            return Move.fromSpawner(singuityIds, arrays.first(self.rootGameState.spawners, lambda s: s.id == discreteMove.spawnerId), discreteMove.playerId)

        return Move.fromPosition(singuityIds, discreteMove.position)