from typing import List, Tuple

import numpy as np
from sklearn.neighbors import KDTree

from clientAis.ais.Artificial import Artificial
from clientAis.communications.GameState import GameState, Singuity, Spawner
from clientAis.communications.ServerCommander import ServerCommander

class propagatingVision(Artificial):
    def __init__(self, serverCommander: ServerCommander):
        super().__init__(serverCommander)

        self.spawnersId: np.ndarray = None
        self.spawners: List[Spawner] = None
        self.spawnerKdtree: KDTree = None
        self.spawnersNeighbourGraph: List[List[int]] = None

    def frame(self, gameState: GameState, currentPlayerId: str):
        self.updateGameStaticInformation(gameState)
        spawnersClosestSinguityIndices = self.groupSinguitiesByClosestSpawner(gameState)

        propagatedPresence = self.getPropagatedPresencePerSpawner(gameState, currentPlayerId, spawnersClosestSinguityIndices)
        spawnerSinguitiesChanged, spawnersClosestSinguityIndices = self.optimizeSpawnerSinguities(propagatedPresence, spawnersClosestSinguityIndices)
        i = 0
        while spawnerSinguitiesChanged and i < 100:
            propagatedPresence = self.getPropagatedPresencePerSpawner(gameState, currentPlayerId, spawnersClosestSinguityIndices)
            spawnerSinguitiesChanged, spawnersClosestSinguityIndices = self.optimizeSpawnerSinguities(propagatedPresence, spawnersClosestSinguityIndices)
            i += 1

        movingCommands = self.getMovingCommands(spawnersClosestSinguityIndices)
        self.executeMovingCommands(movingCommands)

    def updateGameStaticInformation(self, gameState: GameState):
        spawnerIds = np.array([s.id for s in gameState.spawners])
        sortedSpawnerIndices = np.argsort(spawnerIds)
        self.spawners: List[Spawner] = list(np.array(gameState.spawners, dtype=object)[sortedSpawnerIndices])
        sortedSpawnerIds = spawnerIds[sortedSpawnerIndices]

        if self.spawnersId is None or self.spawnersId != sortedSpawnerIds:
            sortedSpawnerPositions = np.array([s.position for s in gameState.spawners])[sortedSpawnerIndices]

            # Generating spawner ordering and kdtree
            self.spawnersId = sortedSpawnerIds
            self.spawnerKdtree = KDTree(sortedSpawnerPositions)

            # Generating spawner neighbour graph
            self.spawnersNeighbourGraph = [[] for _ in range(len(gameState.spawners))]
            potentialNeighbours: List[Tuple[int, int, np.ndarray]] = []

            for i in range(len(gameState.spawners)):
                for j in range(i, len(gameState.spawners)):
                    potentialNeighbours.append((i, j, np.mean([self.spawners[i].position, self.spawners[j].position], axis=0)))

            closestSpawnerToMiddlePoints = self.spawnerKdtree.query([position for _, _, position in potentialNeighbours], return_distance=False)
            neighbours = [(i, j) for closestMiddleSpawner, (i, j, _) in zip(closestSpawnerToMiddlePoints, potentialNeighbours) if
                          closestMiddleSpawner == i or closestMiddleSpawner == j]

            for i, j in neighbours:
                self.spawnersNeighbourGraph[i].append(j)
                self.spawnersNeighbourGraph[j].append(i)

            for i in range(len(self.spawnersNeighbourGraph)):
                self.spawnersNeighbourGraph[i].sort()

    def groupSinguitiesByClosestSpawner(self, gameState: GameState) -> List[List[int]]:
        singuitiesClosestSpawner = self.spawnerKdtree.query([s.position for s in gameState.singuities], return_distance=False)
        spawnerClosestSinguityIndices: List[List[int]] = [[] for _ in range(len(gameState.spawners))]

        for singuityIndex, singuityClosestSpawner in enumerate(singuitiesClosestSpawner):
            spawnerClosestSinguityIndices[singuityClosestSpawner].append(singuityIndex)

        return spawnerClosestSinguityIndices

    def getPropagatedPresencePerSpawner(self, gameState: GameState, currentPlayerId: str, spawnersClosestSinguityIndices: List[List[int]]) -> List[float]:
        # Todo fog of war: remember enemy location through time and add possible enemies at unknown locations

        selfPresences = np.zeros(len(self.spawners))
        singuitiesArray = np.array(gameState.singuities, dtype=object)

        for spawnerIndex, (spawner, spawnerClosestSinguityIndices) in enumerate(zip(self.spawners, spawnersClosestSinguityIndices)):
            closeSinguities: List[Singuity] = singuitiesArray[spawnerClosestSinguityIndices]

            ownSinguities = [s for s in closeSinguities if s.playerId == currentPlayerId]
            enemySinguities = [s for s in closeSinguities if s.playerId != currentPlayerId]

            # Judging quality of singuities using position and health
            selfPresence = np.sum([s.healthPoints / Singuity.MAX_HEALTH_POINT for s in ownSinguities]) / np.sum(np.std([s.position for s in ownSinguities], axis=1))
            selfPresence -= np.sum([s.healthPoints / Singuity.MAX_HEALTH_POINT for s in enemySinguities]) / np.sum(np.std([s.position for s in enemySinguities], axis=1))

            # Judging quality of spawner using allegence
            if spawner.allegence is None:
                selfPresence -= Spawner.REQUIRED_CAPTURING_SINGUITIES
            elif spawner.allegence.playerId != currentPlayerId:
                selfPresence -= Spawner.REQUIRED_CAPTURING_SINGUITIES
                selfPresence -= Spawner.REQUIRED_CAPTURING_SINGUITIES * spawner.allegence.healthPoints / Spawner.MAX_HEALTH_POINTS
            else:
                selfPresence += Spawner.REQUIRED_CAPTURING_SINGUITIES * spawner.allegence.healthPoints / Spawner.MAX_HEALTH_POINTS

            selfPresences[spawnerIndex] = selfPresence

        propagatedPresence = np.zeros(len(self.spawners))
        distanceToSimilarityHp = 50
        spawnerPositions = np.array([s.position for s in self.spawners])

        for spawnerIndex in range(len(self.spawners)):
            distances = np.linalg.norm(spawnerPositions - self.spawners[spawnerIndex], axis=1)
            similarities = distanceToSimilarityHp / (distances + distanceToSimilarityHp)
            propagatedPresence += selfPresences[spawnerIndex] * similarities

        return list(propagatedPresence)

    def optimizeSpawnerSinguities(self, currentPlayerId: str, propagatedPresence: List[int], spawnerClosestSinguityIndices: List[List[int]]):
        def spawnerIndexToProximityToAdvantageousStateChange(spawnerIndex: int) -> float:
            spawner = self.spawners[spawnerIndex]
            if spawner.allegence is None:
                return 0.5
            elif not spawner.allegence.isClaimed:
                return 0.5 + spawner.allegence.healthPoints / (Spawner.MAX_HEALTH_POINTS * 2)
            else: #  spawner.allegence.isClaimed
                return 1 - spawner.allegence.healthPoints / (Spawner.MAX_HEALTH_POINTS * 2)

        spawnersProximityToAdvantageousStateChange = np.array([spawnerIndexToProximityToAdvantageousStateChange(spawnerIndex) for spawnerIndex in range(len(self.spawners))])
        spawnersProximityToAdvantageousStateChange *= propagatedPresence

        for spawnerIndex in np.argsort(spawnersProximityToAdvantageousStateChange):


        return False, spawnerClosestSinguityIndices

    def getMovingCommands(self, spawnersClosestSinguityIndices):
        movingCommands = []

        for spawnerIndex in range(len(self.spawners)):
            assignedSinguities = spawnersClosestSinguityIndices[spawnerIndex]
            spawner = sortedSpawners[spawnerIndex]

            if spawner.allegence is None or (not spawner.allegence.isClaimed and spawner.allegence.playerId == currentPlayerId) or spawner.allegence.playerId != currentPlayerId:
                if singuities.count > spawner.needed or stronghold > 0:
                    movingCommands.append((moveToSpawner, singuities, spawner))

                elif potential > 0:
                    movingCommands.append((moveToLocation, singuities, singuities.mean))

                else:
                    movingCommands.append((moveToSpawner, singuities, self.getClosestOwnSpawner(singuities.mean)))

            else:
                if stronghold > 0:
                    movingCommands.append((moveToSpawner, singuities, spawner))

                elif potential > 0:
                    movingCommands.append((moveToLocation, singuities, singuities.mean))

                else:
                    movingCommands.append((moveToSpawner, singuities, self.getClosestOwnSpawner(singuities.mean)))

        return movingCommands

    def getClosestOwnSpawner(self, position):
        todo
        query
        positive
        strongholds
        todo
        query
        positive
        potentials
        todo
        return least
        negative
        stronghold

    def executeMovingCommands(self, movingCommands):
        for command, *operands in movingCommands:
            if command == "moveToSpawner":
                singuities, spawner = operands
                self.serverCommander.moveUnitsToSpawner([s.id for s in singuities], spawner.id)
            elif command == "moveToPosition":
                singuities, location = operands
                self.serverCommander.moveUnitsToPosition([s.id for s in singuities], location)
