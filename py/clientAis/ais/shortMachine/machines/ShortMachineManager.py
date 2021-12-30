from typing import Dict, List
from uuid import uuid4

import numpy as np

from clientAis.ais.shortMachine.dynamics.Clusterer import Clusterer
from clientAis.ais.shortMachine.gameInterface.Move import Move
from clientAis.ais.shortMachine.long.planProvider import PlanProvider
from clientAis.ais.shortMachine.machines.ShortMachine import ShortMachine
from clientAis.games.GameState import GameState
from utils.arrays import flatten

class ShortMachineManager:
    def __init__(self):
        self.clustersByPlayerId: Dict[str, List[ShortMachine]] = {}
        self.clustererByPlayerId: Dict[str, Clusterer] = {}
        self.planProvider = PlanProvider()

    # todo still attack should also use enemy clusters with a different threshold
    # todo make default movement go to closest claimed spawner
    # todo cluster based on position + speed for less discrepancies in clusters
    # todo defense
    def fromGameState(self, gameState: GameState, currentPlayerId: str) -> List[Move]:
        self.updateClusters(gameState)
        plan = self.planProvider.getPlan(gameState, currentPlayerId)
        clusterProperties = [c.currentState.properties for c in flatten([clusters for clusters in self.clustersByPlayerId.values()])]

        moves = []

        for cluster in self.clustersByPlayerId.get(currentPlayerId, []):
            moves.append(cluster.nextState(gameState, plan, clusterProperties))

        return moves

    def updateClusters(self, gameState: GameState):
        self.clustererByPlayerId = {player.id: self.clustererByPlayerId.get(player.id, Clusterer()) for player in gameState.players}
        self.clustersByPlayerId = {player.id: self.clustersByPlayerId.get(player.id, []) for player in gameState.players}

        for player in gameState.players:
            playerSinguities = np.array([s for s in gameState.singuities if s.playerId == player.id], dtype=object)
            playerSinguityIds = np.array([s.id for s in playerSinguities])
            oldClusterMapping, singuityClusters = self.clustererByPlayerId[player.id].cluster(playerSinguityIds, np.array([s.position for s in playerSinguities]))

            newClusterIdToOldClusterIds = {}
            for oldClusterId, newClusterId in enumerate(oldClusterMapping):
                if newClusterId not in newClusterIdToOldClusterIds:
                    newClusterIdToOldClusterIds[newClusterId] = [oldClusterId]
                else:
                    newClusterIdToOldClusterIds[newClusterId].append(oldClusterId)

            def getShortMachine(newClusterId: int):
                singuities = playerSinguities[singuityClusters == newClusterId].tolist()

                if newClusterId not in newClusterIdToOldClusterIds:
                    return ShortMachine(player.id, singuities)

                oldClusters = [self.clustersByPlayerId[player.id][oldClusterId] for oldClusterId in newClusterIdToOldClusterIds[newClusterId]]

                mostPopulousOldClusterIndex = np.argmax([oldCluster.currentState.properties.singuityCount for oldCluster in oldClusters])

                return oldClusters[mostPopulousOldClusterIndex].updateSinguities(singuities)

            self.clustersByPlayerId[player.id] = [getShortMachine(newClusterId) for newClusterId in np.unique(singuityClusters) if newClusterId != -1]
