from typing import List, Union

import numpy as np
from sklearn.neighbors import KDTree

from clientAis.ais.Artificial import Artificial
from clientAis.communications.GameState import GameState, Spawner
from clientAis.communications.ServerCommander import ServerCommander

class StrongholdVision(Artificial):
    def __init__(self, serverCommander: ServerCommander):
        super().__init__(serverCommander)

        self.spawnersId: Union[np.ndarray, None] = None
        self.spawnersPosition: Union[np.ndarray, None] = None
        self.spawnerKdtree: Union[KDTree, None] = None

    def frame(self, gameState: GameState, currentPlayerId: str):
        spawnerIds = np.array([s.id for s in gameState.spawners])
        sortedSpawnerIndices = np.argsort(spawnerIds)
        sortedSpawnerIds = spawnerIds[sortedSpawnerIndices]
        sortedSpawners: List[Spawner] = list(np.array(gameState.spawners, dtype=object)[sortedSpawnerIndices])

        if self.spawnersId is None or self.spawnersId != sortedSpawnerIds:
            sortedSpawnerPositions = np.array([s.position for s in gameState.spawners])[sortedSpawnerIndices]

            self.spawnersId = sortedSpawnerIds
            self.spawnersPosition = sortedSpawnerPositions
            self.spawnerKdtree = KDTree(self.spawnersPosition)

        ownSinguitiesClosestSpawner = self.spawnerKdtree.query([s.position for s in gameState.singuities if s.playerId == currentPlayerId], return_distance=False)
        enemySinguitiesClosestSpawner = self.spawnerKdtree.query([s.position for s in gameState.singuities if s.playerId != currentPlayerId], return_distance=False)

        spawnerNeighbourGraph: List[List[int]] = [todo]
        spawnerClosestSinguityIndices: List[List[int]] = [todo]
        ownUnitCountPerSpawner = np.array([todo])
        strongholds = np.array([todo])  # Spurious units yield positive numbers, strongholds to be taken and losing strongholds yield negative numbers
        potential_strongholds = np.array([todo])  # Stronghold without the spatial std variable

        ownSinguitiesDestination = [None for s in gameState.singuities if s.playerId == currentPlayerId]

        todo
        # Run objective function with unaltered strongholds
        # Optimize objective function using regroups, defend and attacks

        # Defend strongholds first

        # Regroup second

        # Attack worth enemy spawners last
        takableSpawnersIndices = np.array([spawnerIndex for spawnerIndex, s in enumerate(sortedSpawners) if s.allegence is None or s.allegence.playerId != currentPlayerId or (s.allegence.playerId == currentPlayerId and not s.allegence.isClaimed)])
        sortedTakableSpawnerIndices = takableSpawnersIndices[np.argsort(strongholds[takableSpawnersIndices])]

        for takableSpawnerIndex in sortedTakableSpawnerIndices:
            while ownUnitCountPerSpawner[takableSpawnerIndex] < 100 or strongholds[takableSpawnerIndex] <= 0:

        self.serverCommander.moveUnitsToSpawner(ownSinguityIds, notOwnSpawners[spawnerIndex].id)
