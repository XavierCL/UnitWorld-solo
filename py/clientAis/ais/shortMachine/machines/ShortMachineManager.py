from typing import Dict, List

import numpy as np

from clientAis.ais.shortMachine.dynamics.Clusterer import Clusterer
from clientAis.ais.shortMachine.gameInterface.Move import Move
from clientAis.ais.shortMachine.machines.ShortMachine import ShortMachine
from clientAis.games.GameState import GameState
from utils.arrays import reciprocal

class ShortMachineManager:
    def __init__(self):
        self.clustersByPlayerId: Dict[str, List[ShortMachine]] = {}
        self.clustererByPlayerId: Dict[str, Clusterer] = {}

    def fromGameState(self, gameState: GameState, currentPlayerId: str) -> List[Move]:
        self.updateClusters(gameState, currentPlayerId)

        moves = []

        for cluster in self.clustersByPlayerId.get(currentPlayerId, []):
            moves.append(cluster.nextState(gameState))

        return moves

    def updateClusters(self, gameState: GameState, currentPlayerId: str):
        self.clustererByPlayerId = {player.id: self.clustererByPlayerId.get(player.id, Clusterer()) for player in gameState.players}
        self.clustersByPlayerId = {player.id: self.clustersByPlayerId.get(player.id, []) for player in gameState.players}

        for player in gameState.players:
            playerSinguities = [s for s in gameState.singuities if s.playerId == player.id]
            playerSinguityIds = np.array([s.id for s in playerSinguities])
            oldClusterMapping, singuityClusters = self.clustererByPlayerId[player.id].cluster(playerSinguityIds, np.array([s.position for s in playerSinguities]))
            newClusterMapping = reciprocal(oldClusterMapping)

            def getShortMachine(newClusterId: int):
                singuities = playerSinguityIds[singuityClusters == newClusterId].tolist()

                if newClusterId < len(newClusterMapping) and newClusterMapping[newClusterId] != -1:
                    oldCluster: ShortMachine = self.clustersByPlayerId[player.id][newClusterMapping[newClusterId]]
                    return oldCluster.updateSinguities(singuities)

                return ShortMachine(gameState, currentPlayerId, singuities)

            self.clustersByPlayerId[player.id] = [getShortMachine(newClusterId) for newClusterId in np.unique(singuityClusters)]
