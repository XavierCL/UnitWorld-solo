import math
from typing import List

import numpy as np

class Player:
    def __init__(self, data: dict):
        self.id: str = data["i"]

class SpawnerAllegence:
    def __init__(self, data: dict):
        self.isClaimed: bool = data["i"]
        self.healthPoints: float = data["h"]
        self.playerId: str = data["p"]

class AllegedSpawnerDestination:
    def __init__(self, data: dict):
        self.spawnerId: str = data["i"]
        self.spawnerAllegence: SpawnerAllegence = None if data["a"] == "n" else SpawnerAllegence(data["a"])

class MobileDestination:
    def __init__(self, data: dict):
        self.pointDestination: np.ndarray = None
        self.allegedSpawnerDestination: AllegedSpawnerDestination = None
        self.spawnerDestination: str = None

        if "p" in data:
            self.pointDestination = np.array([data["p"]["x"], data["p"]["y"]])
        elif "s" in data:
            self.allegedSpawnerDestination = AllegedSpawnerDestination(data["s"])
        else:  # "u" in data
            self.spawnerDestination = data["u"]

class Singuity:
    MAX_HEALTH_POINT = 20
    ATTACK_STRENGTH = 5
    ATTACK_FRAME_LAG = 10
    MAXIMUM_UNITS_PER_FRAME = 4

    def __init__(self, data: dict):
        self.id: str = data["i"]
        self.playerId: str = data["p"]
        self.position: np.ndarray = np.array([data["o"]["x"], data["o"]["y"]])
        self.speed: np.ndarray = np.array([data["s"]["x"], data["s"]["y"]])
        self.destination: MobileDestination = None if data["d"] == "n" else MobileDestination(data["d"])
        self.healthPoints: float = data["h"]
        self.lastShootFrame: int = data["l"]

class Spawner:
    MAX_HEALTH_POINTS = 50_000
    REQUIRED_CAPTURING_SINGUITIES = 100
    GESTATION_FRAME_LAG = 900
    SPAWN_FRAME_LAG = 30

    def __init__(self, data: dict):
        self.id: str = data["i"]
        self.position: np.ndarray = np.array([data["p"]["x"], data["p"]["y"]])
        self.allegence: SpawnerAllegence = None if len(data["a"]) == 0 else SpawnerAllegence(data["a"])
        self.rally: MobileDestination = None if len(data["r"]) == 0 else MobileDestination(data["r"])
        self.lastSpawnFrame: int = data["l"]
        self.totalSpawnCount: int = data["t"]
        self.lastClaimFrameCount: int = data["c"]

    def remainingSinguitiesToClaim(self, playerId):
        if self.allegence is None:
            return Spawner.REQUIRED_CAPTURING_SINGUITIES
        elif self.allegence.playerId == playerId:
            if self.allegence.isClaimed:
                return 0
            else:
                return int(math.ceil(Spawner.REQUIRED_CAPTURING_SINGUITIES - Spawner.REQUIRED_CAPTURING_SINGUITIES * self.allegence.healthPoints / Spawner.MAX_HEALTH_POINTS))
        else:
            return Spawner.REQUIRED_CAPTURING_SINGUITIES

class GameState:
    def __init__(self, data: dict):
        self.players: List[Player] = [Player(p) for p in data["p"]]
        self.singuities: List[Singuity] = [Singuity(s) for s in data["s"]]
        self.spawners: List[Spawner] = [Spawner(a) for a in data["a"]]
        self.frameCount: int = data["f"]

class GameStateMessage:
    def __init__(self, data: dict):
        self.gameState: GameState = GameState(data["c"])
        self.currentPlayer: str = data["u"]