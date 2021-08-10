from __future__ import annotations

import math
from typing import Dict, List, Optional

import numpy as np

from clientAis.ais.discreteV1.DiscreteMove import DiscreteMove
from clientAis.ais.discreteV1.Move import Move
from clientAis.games.GameState import GameState, Spawner, SpawnerAllegence
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

    def executeMove(self, move, spawner: DiscreteSpawner) -> DiscretePlayer:
        if move.spawnerId is None:
            return DiscretePlayer(self.id, self.singuityCount, move.position, self.singuitiesStd)

        return DiscretePlayer(self.id, max(self.singuityCount - spawner.remainingSinguitiesToCapture(self.id), 0), spawner.position, self.singuitiesStd)

class DiscreteSpawnerAllegence:
    def __init__(self, isClaimed: bool, playerId: str, healthPoints: float):
        self.isClaimed = isClaimed
        self.playerId = playerId
        self.healthPoints = healthPoints

    @staticmethod
    def fromAllegence(allegence: Optional[SpawnerAllegence]) -> Optional[DiscreteSpawnerAllegence]:
        return None if allegence is None else DiscreteSpawnerAllegence(allegence.isClaimed)

class DiscreteSpawner:
    def __init__(self, id: str, position: np.ndarray, allegence: Optional[DiscreteSpawnerAllegence]):
        self.id = id
        self.position = position
        self.allegence = allegence

    @staticmethod
    def fromSpawner(spawner: Spawner) -> DiscreteSpawner:
        return DiscreteSpawner(spawner.id, spawner.position, DiscreteSpawnerAllegence.fromAllegence(spawner.allegence))

    def executeMove(self, move: DiscreteMove, player: DiscretePlayer) -> DiscreteSpawner:
        if move.spawnerId is not None and move.spawnerId == move.spawnerId:
            remainingSinguities = self.remainingSinguitiesToCapture(player.id)
            if player.singuityCount >= remainingSinguities:
                return DiscreteSpawner(self.id, self.position, DiscreteSpawnerAllegence(True, player.id, Spawner.MAX_HEALTH_POINTS))
            else:
                return DiscreteSpawner(
                    self.id,
                    self.position,
                    DiscreteSpawnerAllegence(
                        False,
                        player.id,
                        Spawner.MAX_HEALTH_POINTS * (Spawner.REQUIRED_CAPTURING_SINGUITIES - remainingSinguities + player.singuityCount) / Spawner.REQUIRED_CAPTURING_SINGUITIES
                    )
                )

        return self

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

class DiscreteGameState:
    def __init__(
        self,
        currentPlayerId: str,
        playerDictionary: Dict[str, DiscretePlayer],
        playerIds: List[str],
        currentPlayerIndex: int,
        spawners: List[DiscreteSpawner],
        spawnersById: Dict[str, DiscreteSpawner],
        gameState: Optional[GameState]
    ):
        self.currentPlayerId = currentPlayerId
        self.playerDictionary = playerDictionary
        self.playerIds = playerIds
        self.currentPlayerIndex = currentPlayerIndex
        self.spawners = spawners
        self.spawnersById = spawnersById
        self.rootGameState = gameState

    @staticmethod
    def fromGameState(gameState: GameState, currentPlayerId: str) -> DiscreteGameState:
        playerCount = len(gameState.players)
        playerDictionary: Dict[str, DiscretePlayer] = [None for _ in range(playerCount)]
        playerIds: List[str] = [None for _ in range(playerCount)]
        currentPlayerIndex = None

        for playerIndex, player in enumerate(gameState.players):
            playerDictionary[player.id] = DiscretePlayer.fromGameState(gameState, player.id)
            playerIds[playerIndex] = player.id

            if player.id == currentPlayerId:
                currentPlayerIndex = playerIndex

        spawners = [DiscreteSpawner.fromSpawner(s) for s in gameState.spawners]
        spawnersById: Dict[str, DiscreteSpawner] = {s.id: s for s in spawners}

        return DiscreteGameState(currentPlayerId, playerDictionary, playerIds, currentPlayerIndex, spawners, spawnersById, gameState)

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

        for move in plan:
            oldSpawner = None
            oldPlayer = getLastPlayerVersion(move.playerId)

            if move.spawnerId is not None:
                oldSpawner = getLastSpawnerVersion(move.spawnerId)
                newSpawnersById[move.spawnerId] = oldSpawner.executeMove(move, oldPlayer)

            newPlayersById[move.playerId] = oldPlayer.executeMove(move, oldSpawner)

        return DiscreteGameState(
            self.currentPlayerId,
            {playerId: getLastPlayerVersion(playerId) for playerId in self.playerIds},
            self.playerIds,
            self.currentPlayerIndex,
            [getLastSpawnerVersion(spawner.id) for spawner in self.spawners],
            {spawner.id: getLastSpawnerVersion(spawner.id) for spawner in self.spawners},
            None
        )

    def discreteMoveToMove(self, discreteMove: DiscreteMove) -> Move:
        singuityIds = [s.id for s in self.rootGameState.singuities if s.playerId == discreteMove.playerId]

        if discreteMove.spawnerId is not None:
            return Move.fromSpawner(singuityIds, arrays.first(self.rootGameState.spawners, lambda s: s.id == discreteMove.spawnerId), discreteMove.playerId)

        return Move.fromPosition(singuityIds, discreteMove.position)
