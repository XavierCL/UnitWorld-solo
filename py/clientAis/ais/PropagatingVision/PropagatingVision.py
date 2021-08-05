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
            selfPresence = self.getSpawnerSelfSinguityPresence(currentPlayerId, closeSinguities)

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

    def getSpawnerSelfSinguityPresence(self, currentPlayerId: str, singuities: List[Singuity]) -> float:
        ownSinguities = [s for s in singuities if s.playerId == currentPlayerId]
        enemySinguities = [s for s in singuities if s.playerId != currentPlayerId]

        selfPresence = np.sum([s.healthPoints / Singuity.MAX_HEALTH_POINT for s in ownSinguities]) / np.sum(np.std([s.position for s in ownSinguities], axis=1))
        selfPresence -= np.sum([s.healthPoints / Singuity.MAX_HEALTH_POINT for s in enemySinguities]) / np.sum(np.std([s.position for s in enemySinguities], axis=1))

        return selfPresence

    def optimizeSpawnerSinguities(self, gameState: GameState, currentPlayerId: str, propagatedPresence: List[int], spawnerClosestSinguityIndices: List[List[int]]):
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

        spawnerPositions = np.array([s.position for s in self.spawners])
        singuitiesArray = np.array(gameState.singuities, dtype=object)

        for spawnerIndex in np.argsort(spawnersProximityToAdvantageousStateChange):
            if self.getSpawnerSelfSinguityPresence(currentPlayerId, list(singuitiesArray[spawnerClosestSinguityIndices[spawnerIndex]])) < 0:

                distances = np.linalg.norm(spawnerPositions - self.spawners[spawnerIndex], axis=1)

                # Skipping first because it's always the current spawner, which already contains its singuities
                for potentialNeighbourSpawnerIndex in np.argsort(distances)[1:]:
                    neighbourSinguities = list(singuitiesArray[spawnerClosestSinguityIndices[potentialNeighbourSpawnerIndex]])
                    if self.getSpawnerSelfSinguityPresence(currentPlayerId,neighbourSinguities) > 0:
                        spawnerClosestSinguityIndices[spawnerIndex].extend([index for index, singuity in zip(spawnerClosestSinguityIndices[potentialNeighbourSpawnerIndex], neighbourSinguities) if singuity.playerId==currentPlayerId])
                        spawnerClosestSinguityIndices[potentialNeighbourSpawnerIndex] =[index for index, singuity in zip(spawnerClosestSinguityIndices[potentialNeighbourSpawnerIndex], neighbourSinguities) if singuity.playerId!=currentPlayerId]

                        return True, spawnerClosestSinguityIndices

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
