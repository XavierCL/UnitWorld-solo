from typing import Any, List, Tuple

import numpy as np
from sklearn.neighbors import KDTree

from clientAis.ais.Artificial import Artificial
from clientAis.communications.ServerCommander import ServerCommander
from clientAis.games.GameState import GameState, Singuity, Spawner, SpawnerAllegence
from utils import arrays

class PropagatingVisionAi(Artificial):
    def __init__(self, serverCommander: ServerCommander):
        super().__init__(serverCommander)

        self.spawnersId: np.ndarray = None
        self.spawners: List[Spawner] = None
        self.spawnerKdtree: KDTree = None

    def frame(self, gameState: GameState, currentPlayerId: str):
        self.updateGameStaticInformation(gameState)
        spawnersClosestSinguityIndices = self.groupSinguitiesByClosestSpawner(gameState)

        propagatedPresence = self.getPropagatedPresencePerSpawner(gameState, currentPlayerId, spawnersClosestSinguityIndices)
        spawnerSinguitiesChanged, spawnersClosestSinguityIndices = self.optimizeSpawnerSinguities(gameState, currentPlayerId, propagatedPresence, spawnersClosestSinguityIndices)

        i = 0
        while spawnerSinguitiesChanged and i < len(self.spawners) ** 2:
            propagatedPresence = self.getPropagatedPresencePerSpawner(gameState, currentPlayerId, spawnersClosestSinguityIndices)
            spawnerSinguitiesChanged, spawnersClosestSinguityIndices = self.optimizeSpawnerSinguities(
                gameState, currentPlayerId, propagatedPresence, spawnersClosestSinguityIndices
            )
            i += 1

        movingCommands = self.getMovingCommands(gameState, currentPlayerId, spawnersClosestSinguityIndices, propagatedPresence)
        self.executeMovingCommands(movingCommands)

    def updateGameStaticInformation(self, gameState: GameState):
        spawnerIds = np.array([s.id for s in gameState.spawners])
        sortedSpawnerIndices = np.argsort(spawnerIds)
        self.spawners: List[Spawner] = list(np.array(gameState.spawners, dtype=object)[sortedSpawnerIndices])
        sortedSpawnerIds = spawnerIds[sortedSpawnerIndices]

        if self.spawnersId is None or len(self.spawnersId) != len(sortedSpawnerIds) or np.any(self.spawnersId != sortedSpawnerIds):
            sortedSpawnerPositions = np.array([s.position for s in gameState.spawners])[sortedSpawnerIndices]

            # Generating spawner ordering and kdtree
            self.spawnersId = sortedSpawnerIds
            self.spawnerKdtree = KDTree(sortedSpawnerPositions)

    def groupSinguitiesByClosestSpawner(self, gameState: GameState) -> List[List[int]]:
        singuitiesClosestSpawner = self.spawnerKdtree.query([s.position for s in gameState.singuities], return_distance=False)
        spawnerClosestSinguityIndices: List[List[int]] = [[] for _ in range(len(gameState.spawners))]

        for singuityIndex, singuityClosestSpawner in enumerate(singuitiesClosestSpawner):
            spawnerClosestSinguityIndices[singuityClosestSpawner.item()].append(singuityIndex)

        return spawnerClosestSinguityIndices

    def getPropagatedPresencePerSpawner(self, gameState: GameState, currentPlayerId: str, spawnersClosestSinguityIndices: List[List[int]]) -> List[float]:
        # Todo fog of war: remember enemy location through time and add possible enemies at unknown locations

        selfPresences = np.zeros(len(self.spawners))
        singuitiesArray = np.array(gameState.singuities, dtype=object)

        for spawnerIndex, (spawner, spawnerClosestSinguityIndices) in enumerate(zip(self.spawners, spawnersClosestSinguityIndices)):
            selfPresences[spawnerIndex] = self.getSpawnerSelfSinguityPresence(currentPlayerId, singuitiesArray[spawnerClosestSinguityIndices])

        propagatedPresence = np.zeros(len(self.spawners))

        spawnerPositions = np.array([s.position for s in self.spawners])
        singuitiesPosition = [np.mean([s.position for s in singuitiesArray[singuityIndices]], axis=0) if len(singuityIndices) > 0 else spawnerPositions[spawnerIndex] for
                              spawnerIndex, singuityIndices in enumerate(spawnersClosestSinguityIndices)]

        for spawnerIndex in range(len(self.spawners)):
            distances = np.linalg.norm(spawnerPositions - singuitiesPosition[spawnerIndex], axis=1)
            similarities = 100 / (distances + 100)
            propagatedPresence += selfPresences[spawnerIndex] * similarities

        return list(propagatedPresence)

    def getSpawnerSelfSinguityPresence(self, currentPlayerId: str, singuities: List[Singuity], use_spatial_distribution=True) -> float:
        ownSinguities = [s for s in singuities if s.playerId == currentPlayerId]
        enemySinguities = [s for s in singuities if s.playerId != currentPlayerId]

        selfPresence = 0 if len(ownSinguities) == 0 else (np.sum([s.healthPoints / Singuity.MAX_HEALTH_POINT for s in ownSinguities]) / (
            np.linalg.norm(np.std([s.position for s in ownSinguities], axis=0)) + 1 if use_spatial_distribution else 1))
        selfPresence -= 0 if len(enemySinguities) == 0 else (np.sum([s.healthPoints / Singuity.MAX_HEALTH_POINT for s in enemySinguities]) / (
            np.linalg.norm(np.std([s.position for s in enemySinguities], axis=0)) + 1 if use_spatial_distribution else 1))

        return selfPresence

    def optimizeSpawnerSinguities(self, gameState: GameState, currentPlayerId: str, propagatedPresence: List[float], spawnerClosestSinguityIndices: List[List[int]]):
        def spawnerIndexToProximityToAdvantageousStateChange(spawnerIndex: int) -> float:
            spawner = self.spawners[spawnerIndex]
            if spawner.allegence is None:
                return 0.5
            elif not spawner.allegence.isClaimed:
                return 0.5 + spawner.allegence.healthPoints / (Spawner.MAX_HEALTH_POINTS * 2)
            else:  # spawner.allegence.isClaimed
                return 1 - spawner.allegence.healthPoints / (Spawner.MAX_HEALTH_POINTS * 2)

        spawnersProximityToAdvantageousStateChange = np.array([spawnerIndexToProximityToAdvantageousStateChange(spawnerIndex) for spawnerIndex in range(len(self.spawners))])
        spawnersProximityToAdvantageousStateChange *= propagatedPresence

        spawnerPositions = np.array([s.position for s in self.spawners])
        singuitiesArray = np.array(gameState.singuities, dtype=object)

        spawnerRequiredCounts = np.zeros(len(self.spawners), dtype=np.int32)

        for spawnerIndex in np.argsort(spawnersProximityToAdvantageousStateChange)[::-1]:

            spawnerRequiredCounts[spawnerIndex] = int(self.spawners[spawnerIndex].remainingSinguitiesToClaim(currentPlayerId) * 1.5)

            if self.getSpawnerSelfSinguityPresence(currentPlayerId, list(singuitiesArray[spawnerClosestSinguityIndices[spawnerIndex]])) < 0\
                    or self.spawners[spawnerIndex].remainingSinguitiesToClaim(currentPlayerId) > len(
                [s for s in singuitiesArray[spawnerClosestSinguityIndices[spawnerIndex]] if s.playerId == currentPlayerId]
                ):

                distances = np.linalg.norm(spawnerPositions - self.spawners[spawnerIndex].position, axis=1)

                # Skipping first because it's always the current spawner, which already contains its singuities
                for potentialNeighbourSpawnerIndex in np.argsort(distances)[1::]:

                    neighbourSinguities: List[Singuity] = singuitiesArray[spawnerClosestSinguityIndices[potentialNeighbourSpawnerIndex]]
                    ownUsableNeighbourSinguities = arrays.soft_accessor(
                        [(singuityIndex, singuity) for singuityIndex, singuity in zip(spawnerClosestSinguityIndices[potentialNeighbourSpawnerIndex], neighbourSinguities) if
                         singuity.playerId == currentPlayerId], spawnerRequiredCounts[potentialNeighbourSpawnerIndex]
                        )

                    if self.getSpawnerSelfSinguityPresence(currentPlayerId, neighbourSinguities) > 0 and len(ownUsableNeighbourSinguities) > 0:

                        enemyNeighbourSinguities = [(singuityIndex, singuity) for singuityIndex, singuity in
                                                    zip(spawnerClosestSinguityIndices[potentialNeighbourSpawnerIndex], neighbourSinguities) if singuity.playerId != currentPlayerId]

                        ownUnusableNeighbourSinguities = arrays.soft_accessor(
                            [(singuityIndex, singuity) for singuityIndex, singuity in zip(spawnerClosestSinguityIndices[potentialNeighbourSpawnerIndex], neighbourSinguities) if
                             singuity.playerId == currentPlayerId], 0, spawnerRequiredCounts[potentialNeighbourSpawnerIndex]
                            )

                        spawnerClosestSinguityIndices[spawnerIndex].extend([index for index, _ in ownUsableNeighbourSinguities])
                        spawnerClosestSinguityIndices[potentialNeighbourSpawnerIndex] = [index for index, _ in enemyNeighbourSinguities + ownUnusableNeighbourSinguities]

                        return True, spawnerClosestSinguityIndices

        return False, spawnerClosestSinguityIndices

    def getMovingCommands(self, gameState: GameState, currentPlayerId: str, spawnersClosestSinguityIndices: List[List[int]], propagatedPresence: List[float]):
        movingCommands = []

        singuitiesArray = np.array(gameState.singuities, dtype=object)

        for spawnerIndex in range(len(self.spawners)):
            assignedSinguityIndices: List[int] = spawnersClosestSinguityIndices[spawnerIndex]
            assignedSinguities: List[Singuity] = singuitiesArray[assignedSinguityIndices]
            ownAssignedSinguities = [s for s in assignedSinguities if s.playerId == currentPlayerId]

            if len(ownAssignedSinguities) == 0:
                continue

            spawner: Spawner = self.spawners[spawnerIndex]
            allegence: SpawnerAllegence = spawner.allegence

            if allegence is None or (not allegence.isClaimed and allegence.playerId == currentPlayerId) or allegence.playerId != currentPlayerId:
                requiredUnits = Spawner.REQUIRED_CAPTURING_SINGUITIES\
                    if allegence is None\
                    else (
                    Spawner.REQUIRED_CAPTURING_SINGUITIES * (Spawner.MAX_HEALTH_POINTS - allegence.healthPoints) / Spawner.MAX_HEALTH_POINTS
                    if allegence.playerId == currentPlayerId
                    else Spawner.REQUIRED_CAPTURING_SINGUITIES
                )

                if len([s for s in singuitiesArray[assignedSinguityIndices] if s.playerId == currentPlayerId]) > requiredUnits\
                        and self.getSpawnerSelfSinguityPresence(currentPlayerId, assignedSinguities, use_spatial_distribution=True) > 0:
                    movingCommands.append(("moveToSpawner", ownAssignedSinguities, spawner))

                elif self.getSpawnerSelfSinguityPresence(currentPlayerId, assignedSinguities, use_spatial_distribution=False) > 0:
                    movingCommands.append(("moveToPosition", ownAssignedSinguities, np.mean([s.position for s in ownAssignedSinguities], axis=0)))

                else:
                    movingCommands.append(
                        ("moveToPosition", ownAssignedSinguities,
                         self.getClosestOwnSpawner(np.mean([s.position for s in ownAssignedSinguities], axis=0), propagatedPresence).position)
                    )

            else:
                if self.getSpawnerSelfSinguityPresence(currentPlayerId, assignedSinguities, use_spatial_distribution=True) > 0:
                    movingCommands.append(("moveToPosition", ownAssignedSinguities, spawner.position))

                elif self.getSpawnerSelfSinguityPresence(currentPlayerId, assignedSinguities, use_spatial_distribution=False) > 0:
                    movingCommands.append(("moveToPosition", ownAssignedSinguities, np.mean([s.position for s in ownAssignedSinguities], axis=0)))

                else:
                    movingCommands.append(
                        ("moveToPosition", ownAssignedSinguities,
                         self.getClosestOwnSpawner(np.mean([s.position for s in ownAssignedSinguities], axis=0), propagatedPresence).position)
                    )

        return movingCommands

    def getClosestOwnSpawner(self, position: np.array, propagatedPresence: List[float]) -> Spawner:
        distances = np.linalg.norm(np.array([s.position for s in self.spawners]) - position, axis=1)
        for nextClosestSpawnerIndex in np.argsort(distances):
            if propagatedPresence[nextClosestSpawnerIndex] > 0:
                return self.spawners[nextClosestSpawnerIndex]

        maximalPresenceSpawners = np.nonzero(propagatedPresence == np.max(propagatedPresence))[0]
        return self.spawners[maximalPresenceSpawners[np.argmin(distances[maximalPresenceSpawners])]]

    def executeMovingCommands(self, movingCommands: List[Tuple[str, List[Singuity], Any]]):
        for command, singuities, operand in movingCommands:
            if command == "moveToSpawner":
                spawner: Spawner = operand
                self.serverCommander.moveUnitsToSpawner([s.id for s in singuities], spawner.id)
            elif command == "moveToPosition":
                location: np.ndarray = operand
                self.serverCommander.moveUnitsToPosition([s.id for s in singuities], location)
            else:
                raise Exception("Movement type does not exist")
