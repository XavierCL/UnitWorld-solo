from __future__ import annotations

import math
from typing import Any, Callable, Dict, List, Optional, Tuple

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

    def executeMove(self, move: DiscreteMove, getLastSpawnerVersion: Callable[[str], DiscreteSpawner], setSpawner: Callable[[str, DiscreteSpawner], Any], frameBeforeMove: int, restrictedDuration: int = None) -> Tuple[int, DiscretePlayer]:
        targetPosition = move.position if move.position is not None else getLastSpawnerVersion(move.spawnerId).position
        movementDuration = PhysicsEstimator.estimateMovementDuration(self.singuitiesMeanPosition, targetPosition)

        def getLastingMoveDuration(moveDuration: int) -> int:
            if move.minimumMoveTime is not None and move.minimumMoveTime > moveDuration:
                return move.minimumMoveTime

            return moveDuration

        if movementDuration > restrictedDuration:
            return restrictedDuration, DiscretePlayer(self.id, self.singuityCount, self.singuitiesMeanPosition + (targetPosition - self.singuitiesMeanPosition) * (restrictedDuration / movementDuration), self.singuitiesStd)

        if move.spawnerId is None:
            return getLastingMoveDuration(movementDuration), DiscretePlayer(self.id, self.singuityCount, targetPosition, self.singuitiesStd)
        else:
            spawner = getLastSpawnerVersion(move.spawnerId)

            if spawner.isClaimed() and spawner.isAllegedToPlayer(move.playerId):
                return getLastingMoveDuration(movementDuration), DiscretePlayer(self.id, self.singuityCount, targetPosition, self.singuitiesStd)

            else:
                durationToDeath = PhysicsEstimator.estimateSpawnerToZeroHealthDuration(self.singuityCount, spawner.getHealthPoints())

                if movementDuration + durationToDeath > restrictedDuration:
                    setSpawner(spawner.id, spawner.attackedFor(self.singuityCount, restrictedDuration - movementDuration))
                    return restrictedDuration, DiscretePlayer(self.id, self.singuityCount, spawner.position, self.singuitiesStd)

                setSpawner(spawner.id, spawner.tryClaimedBy(self.id, self.singuityCount, frameBeforeMove + movementDuration))

                return (
                    getLastingMoveDuration(movementDuration + durationToDeath),
                    DiscretePlayer(self.id, max(self.singuityCount - spawner.remainingSinguitiesToCapture(self.id), 0), spawner.position, self.singuitiesStd)
                )

    def appendNewSpawned(self, spawner, anteMoveFrameCount: int, postMoveFrameCount: int) -> DiscretePlayer:
        matureSpawnerDuration = min(postMoveFrameCount - anteMoveFrameCount, postMoveFrameCount - spawner.lastFrameClaimed + Spawner.GESTATION_FRAME_LAG)
        durationToTarget = PhysicsEstimator.estimateMovementDuration(spawner.position, self.singuitiesMeanPosition)
        atTargetCount = max((matureSpawnerDuration - durationToTarget) * PhysicsEstimator.SPAWNER_SPAWN_PER_FRAME, 0)
        furthestSinguityRatio = min(matureSpawnerDuration * Singuity.MAXIMUM_UNITS_PER_FRAME / np.linalg.norm(self.singuitiesMeanPosition - spawner.position), 1)
        unitPositionInLine = (spawner.position + (self.singuitiesMeanPosition - spawner.position) * furthestSinguityRatio / 2)
        unitCountInLine = PhysicsEstimator.distanceToSpawningSinguities(np.linalg.norm(self.singuitiesMeanPosition - spawner.position) * furthestSinguityRatio)
        singuitiesInLineStd = np.linalg.norm(self.singuitiesMeanPosition - spawner.position) * furthestSinguityRatio / 4

        newSinguitiesMean, newSinguitiesStd, newSinguitiesCount = arrays.combineMeanStdAndCount(
            self.singuitiesMeanPosition, np.ones(1), atTargetCount, unitPositionInLine, singuitiesInLineStd, unitCountInLine
        )

        singuitiesMean, singuitiesStd, singuitiesCount = arrays.combineMeanStdAndCount(
            self.singuitiesMeanPosition, np.array([self.singuitiesStd]), self.singuityCount, newSinguitiesMean, np.linalg.norm(newSinguitiesStd), newSinguitiesCount
        )

        return DiscretePlayer(self.id, singuitiesCount, singuitiesMean, np.linalg.norm(singuitiesStd))

class DiscreteSpawnerAllegence:
    def __init__(self, isClaimed: bool, playerId: str, healthPoints: float):
        self.isClaimed = isClaimed
        self.playerId = playerId
        self.healthPoints = healthPoints

    @staticmethod
    def fromAllegence(allegence: Optional[SpawnerAllegence]) -> Optional[DiscreteSpawnerAllegence]:
        return None if allegence is None else DiscreteSpawnerAllegence(allegence.isClaimed, allegence.playerId, allegence.healthPoints)

    def loseHealth(self, health: float) -> Optional[DiscreteSpawnerAllegence]:
        if health > self.healthPoints:
            return None

        return DiscreteSpawnerAllegence(self.isClaimed, self.playerId, self.healthPoints - health)

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

    def attackedFor(self, singuityCount, duration) -> DiscreteSpawner:
        remainingHealth = PhysicsEstimator.estimateRemainingHealthAfterNFrame(singuityCount, self.getHealthPoints(), duration, 0, False)
        return DiscreteSpawner(self.id, self.position, None if remainingHealth == 0 else self.allegence.loseHealth(self.getHealthPoints() - remainingHealth), self.lastFrameClaimed)

    def tryClaimedBy(self, playerId, singuityCount, frameCount: int) -> DiscreteSpawner:
        if self.remainingSinguitiesToCapture(playerId) <= singuityCount:
            return DiscreteSpawner(self.id, self.position, DiscreteSpawnerAllegence(True, playerId, Spawner.MAX_HEALTH_POINTS), frameCount)

        claimedSinguitiesAfterFrame = Spawner.REQUIRED_CAPTURING_SINGUITIES - self.remainingSinguitiesToCapture(playerId) + singuityCount
        return DiscreteSpawner(self.id, self.position, DiscreteSpawnerAllegence(False, playerId, Spawner.MAX_HEALTH_POINTS * claimedSinguitiesAfterFrame / Spawner.REQUIRED_CAPTURING_SINGUITIES), self.lastFrameClaimed)

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

    def executePlan(self, plan: List[DiscreteMove], restrictedFrameCount: int = None) -> DiscreteGameState:
        if restrictedFrameCount <= self.frameCount:
            return self

        restrictedDuration = restrictedFrameCount - self.frameCount

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
            moveDuration, newPlayersById[move.playerId] = oldPlayer.executeMove(move, getLastSpawnerVersion, lambda sid, s: arrays.assignInline(newSpawnersById, sid, s), self.frameCount, restrictedDuration=restrictedDuration)

        # Assign new spawned singuities
        for spawner in self.spawners:
            if spawner.isClaimed():
                newPlayersById[spawner.allegence.playerId] = getLastPlayerVersion(spawner.allegence.playerId).appendNewSpawned(
                    spawner, self.frameCount, self.frameCount + moveDuration
                )

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

        elif discreteMove.position is not None:
            return Move.fromPosition(singuityIds, discreteMove.position)

        else:
            return Move.fromNothing()
