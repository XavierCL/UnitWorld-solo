import numpy as np
from sklearn.neighbors import KDTree

from clientAis.ais.Artificial import Artificial
from clientAis.communications.GameState import GameState

class SingleMindClosest(Artificial):
    def frame(self, gameState: GameState, currentPlayerId: str):
        notOwnSpawners = [s for s in gameState.spawners if s.allegence is None or not s.allegence.isClaimed or (s.allegence.isClaimed and s.allegence.playerId != currentPlayerId)]
        notOwnSpawnersPosition = [s.position for s in notOwnSpawners]
        notOwnSpawnerKdtree = KDTree(np.array(notOwnSpawnersPosition))

        ownSinguities = [s for s in gameState.singuities if s.playerId == currentPlayerId]
        ownSinguitiesMeanPosition = np.mean([s.position for s in ownSinguities], axis=0)

        spawnerIndex = notOwnSpawnerKdtree.query(ownSinguitiesMeanPosition.reshape(1, -1), return_distance=False)[0].item()

        ownSinguityIds = [s.id for s in ownSinguities]

        self.gameController.moveUnitsToSpawner(ownSinguityIds, notOwnSpawners[spawnerIndex].id)