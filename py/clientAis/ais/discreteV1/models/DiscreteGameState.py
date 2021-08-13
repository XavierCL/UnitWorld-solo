from __future__ import annotations

import math
from typing import Dict, List, Optional

import numpy as np

from clientAis.games.GameState import GameState, Spawner, SpawnerAllegence

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