from typing import List, Union

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
        self.spawnerAllegence: Union[SpawnerAllegence, None] = None if data["a"] == "n" else SpawnerAllegence(data["a"])

class MobileDestination:
    def __init__(self, data: dict):
        self.pointDestination: Union[List[int], None] = None
        self.allegedSpawnerDestination: Union[AllegedSpawnerDestination, None] = None
        self.spawnerDestination: Union[str, None] = None

        if "p" in data:
            self.pointDestination = [data["p"]["x"], data["p"]["y"]]
        elif "s" in data:
            self.allegedSpawnerDestination = AllegedSpawnerDestination(data["s"])
        else:  # "i" in data
            self.spawnerDestination = data["i"]

class Singuity:
    def __init__(self, data: dict):
        self.id: str = data["i"]
        self.playerId: str = data["p"]
        self.position: List[float] = [data["o"]["x"], data["o"]["y"]]
        self.speed: List[float] = [data["s"]["x"], data["s"]["y"]]
        self.destination: Union[MobileDestination, None] = None if data["d"] == "n" else MobileDestination(data["d"])
        self.healthPoints: float = data["h"]
        self.lastShootFrame: int = data["l"]

class Spawner:
    def __init__(self, data: dict):
        self.id: str = data["i"]
        self.position: List[float] = [data["p"]["x"], data["p"]["y"]]
        self.allegence: Union[SpawnerAllegence, None] = None if len(data["a"]) == 0 else SpawnerAllegence(data["a"])
        self.rally: Union[MobileDestination, None] = None if len(data["r"]) == 0 else MobileDestination(data["r"])
        self.lastSpawnFrame: int = data["l"]
        self.totalSpawnCount: int = data["t"]
        self.lastClaimFrameCount: int = data["c"]

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
