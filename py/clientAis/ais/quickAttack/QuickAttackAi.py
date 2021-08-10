import numpy as np
from sklearn.neighbors import KDTree

from clientAis.ais.Artificial import Artificial
from clientAis.games.GameState import GameState, Spawner

class QuickAttackAi(Artificial):
    def frame(self, gameState: GameState, currentPlayerId: str):
        if self.frameIsIgnored(gameState, currentPlayerId):
            return

        targetEnemySpawner = self.getEnemySpawnerTarget(gameState, currentPlayerId)
        self.attackTarget(gameState, currentPlayerId, targetEnemySpawner)

    def frameIsIgnored(self, gameState: GameState, currentPlayerId: str) -> bool:
        enemySpawners = [s for s in gameState.spawners if s.allegence is not None and s.allegence.isClaimed and s.allegence.playerId != currentPlayerId]

        if len(enemySpawners) == 0:
            # Game is won if fog of war is disabled
            return True

        ownSpawners = [s for s in gameState.spawners if s.allegence is not None and s.allegence.isClaimed and s.allegence.playerId == currentPlayerId]

        if len(ownSpawners) == 0:
            # Game is lost
            return True

        ownSinguities = [s for s in gameState.singuities if s.playerId == currentPlayerId]

        if len(ownSinguities) == 0:
            # No units to control
            return True

        return False

    def getEnemySpawnerTarget(self, gameState: GameState, currentPlayerId: str) -> Spawner:
        ownSinguities = [s for s in gameState.singuities if s.playerId == currentPlayerId]
        ownSinguitiesPosition = np.array([s.position for s in ownSinguities])
        ownSinguitiesMedianPosition = np.median(ownSinguitiesPosition, axis=0)

        enemySpawners = [s for s in gameState.spawners if s.allegence is not None and s.allegence.isClaimed and s.allegence.playerId != currentPlayerId]
        enemySpawnersPosition = [s.position for s in enemySpawners]
        enemySpawnerKdtree = KDTree(np.array(enemySpawnersPosition))
        enemyTargetSpawnerIndex = enemySpawnerKdtree.query(ownSinguitiesMedianPosition.reshape(1, -1), return_distance=False)[0].item()
        return enemySpawners[enemyTargetSpawnerIndex]

    def attackTarget(self, gameState: GameState, currentPlayerId: str, target: Spawner):
        self.serverCommander.moveUnitsToPosition([s.id for s in gameState.singuities if s.playerId == currentPlayerId], target.position)
