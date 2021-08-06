import numpy as np
from sklearn.cluster import DBSCAN
from sklearn.neighbors import KDTree

from clientAis.ais.Artificial import Artificial
from clientAis.communications.GameState import GameState, Spawner

class PackingAi(Artificial):
    def frame(self, gameState: GameState, currentPlayerId: str):
        if self.frameIsIgnored(gameState, currentPlayerId):
            return

        targetEnemySpawner = self.getEnemySpawnerTarget(gameState, currentPlayerId)
        self.attackTarget(gameState, currentPlayerId, targetEnemySpawner)

    def frameIsIgnored(self, gameState: GameState, currentPlayerId: str) -> bool:
        notOwnSpawners = [s for s in gameState.spawners if s.allegence is None or not s.allegence.isClaimed or (s.allegence.isClaimed and s.allegence.playerId != currentPlayerId)]

        if len(notOwnSpawners) == 0:
            # Game is won
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

        notOwnSpawners = [s for s in gameState.spawners if s.allegence is None or not s.allegence.isClaimed or (s.allegence.isClaimed and s.allegence.playerId != currentPlayerId)]
        notOwnSpawnersPosition = [s.position for s in notOwnSpawners]
        notOwnSpawnerKdtree = KDTree(np.array(notOwnSpawnersPosition))
        enemyTargetSpawnerIndex = notOwnSpawnerKdtree.query(ownSinguitiesMedianPosition.reshape(1, -1), return_distance=False)[0].item()
        return notOwnSpawners[enemyTargetSpawnerIndex]

    def attackTarget(self, gameState: GameState, currentPlayerId: str, target: Spawner):
        ownSinguities = [s for s in gameState.singuities if s.playerId == currentPlayerId]
        ownSinguitiesPosition = np.array([s.position for s in ownSinguities])

        ownSpawners = [s for s in gameState.spawners if s.allegence is not None and s.allegence.isClaimed and s.allegence.playerId == currentPlayerId]
        ownSpawnersPosition = [s.position for s in ownSpawners]
        ownSpawnerKdtree = KDTree(np.array(ownSpawnersPosition))
        ownSpawnerIndexRallyPoint = ownSpawnerKdtree.query(target.position.reshape(1, -1), return_distance=False)[0].item()
        rallyPoint = ownSpawners[ownSpawnerIndexRallyPoint].position

        clusterer = DBSCAN(eps=100, min_samples=target.remainingSinguitiesToClaim(currentPlayerId))
        clusters = clusterer.fit_predict(ownSinguitiesPosition)
        clusteredSinguities = clusters != -1
        ownSinguitiesIdSentToTarget = [s.id for s in np.array(ownSinguities, dtype=object)[clusteredSinguities]]

        if target.allegence is None or (not target.allegence.isClaimed and target.allegence.playerId == currentPlayerId):
            self.serverCommander.moveUnitsToSpawner(ownSinguitiesIdSentToTarget, target.id)
        else:
            self.serverCommander.moveUnitsToPosition(ownSinguitiesIdSentToTarget, target.position)

        ownSinguitiesIdNotSentToTarget = [s.id for s in np.array(ownSinguities, dtype=object)[~clusteredSinguities]]
        self.serverCommander.moveUnitsToPosition(ownSinguitiesIdNotSentToTarget, rallyPoint)
