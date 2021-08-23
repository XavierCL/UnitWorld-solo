from __future__ import annotations

from typing import Dict, List, Optional, Tuple

from sklearn.neighbors import KDTree

from clientAis.ais.discreteV1.models.DiscretePlayer import DiscretePlayer
from clientAis.ais.discreteV1.models.DiscreteSpawner import DiscreteSpawner
from clientAis.ais.discreteV1.Move import Move
from clientAis.ais.discreteV1.PhysicsEstimator import PhysicsEstimator
from clientAis.ais.discreteV1.Plans.DiscreteMove import DiscreteMove
from clientAis.games.GameState import GameState
from utils import arrays

class DiscreteGameState:
    spawnerCollisionDetector: KDTree = None

    def __init__(
        self,
        currentPlayerId: str,
        playerDictionary: Dict[str, DiscretePlayer],
        spawners: List[DiscreteSpawner],
        spawnersById: Dict[str, DiscreteSpawner],
        frameCount: int,
        gameState: Optional[GameState]
    ):
        self.currentPlayerId = currentPlayerId
        self.playerDictionary = playerDictionary
        self.spawners = spawners
        self.spawnersById = spawnersById
        self.frameCount = frameCount
        self.rootGameState = gameState

    @staticmethod
    def fromGameState(gameState: GameState, currentPlayerId: str) -> DiscreteGameState:
        playerDictionary: Dict[str, DiscretePlayer] = {}
        playerIds: List[str] = []
        DiscreteGameState.spawnerCollisionDetector = KDTree([s.position for s in gameState.spawners])

        for playerIndex, player in enumerate(gameState.players):
            discretePlayer = DiscretePlayer.fromGameState(gameState, player.id)

            if discretePlayer.singuityCount == 0 and len([s for s in gameState.spawners if s.allegence is not None and s.allegence.isClaimed and s.allegence.playerId == player.id]) == 0:
                continue

            playerDictionary[player.id] = discretePlayer
            playerIds.append(player.id)

        spawners = [DiscreteSpawner.fromSpawner(s) for s in gameState.spawners]
        spawnersById: Dict[str, DiscreteSpawner] = {s.id: s for s in spawners}

        return DiscreteGameState(currentPlayerId, playerDictionary, spawners, spawnersById, gameState.frameCount, gameState)

    def executeMove(self, move: DiscreteMove, restrictedFrameCount: Optional[int] = None) -> DiscreteGameState:
        restrictedDuration = None if (restrictedFrameCount is None) else (restrictedFrameCount - self.frameCount)

        if restrictedFrameCount is not None and restrictedFrameCount <= self.frameCount:
            return self

        newPlayersById: Dict[str, DiscretePlayer] = {}
        newSpawnersById: Dict[str, DiscreteSpawner] = {}

        def getLastPlayerVersion(playerId: str) -> DiscretePlayer:
            if playerId in newPlayersById:
                return newPlayersById[playerId]

            return self.playerDictionary[playerId]

        def getLastSpawnerVersion(spawnerId: str) -> DiscreteSpawner:
            if spawnerId in newSpawnersById:
                return newSpawnersById[spawnerId]

            return self.spawnersById[spawnerId]

        def updateSpawnersAndPlayers(updatedSpawners: List[DiscreteSpawner], updatedPlayers: List[DiscretePlayer]):
            for updatedSpawner in updatedSpawners:
                newSpawnersById[updatedSpawner.id] = updatedSpawner

            for updatedPlayer in updatedPlayers:
                newPlayersById[updatedPlayer.id] = updatedPlayer

        def assignNewSpawnees(frameBefore: int, frameAfter: int):
            for spawner in self.spawners:
                if spawner.isClaimed():
                    newPlayersById[spawner.allegence.playerId] = getLastPlayerVersion(spawner.allegence.playerId).appendNewSpawned(
                        spawner.position, spawner.lastFrameClaimed, frameBefore, frameAfter
                    )

        # Get time until cluster arrives
        movementDuration, movedPlayer = getLastPlayerVersion(move.playerId).executeMovement(move, move.position if move.spawnerId is None else getLastSpawnerVersion(move.spawnerId).position, restrictedDuration)

        # Process interactions until player arrives
        updatedSpawners, updatedPlayers, interactionDuration = self.executeInteractions(self.spawners, [p for p in self.playerDictionary.values() if p.id != move.playerId], movementDuration)

        # Update both interactions and moved player
        updateSpawnersAndPlayers(updatedSpawners, updatedPlayers + movedPlayer)

        # Spawn singuities before player fights
        assignNewSpawnees(self.frameCount, self.frameCount + movementDuration)

        # Process main interactions once player is at destination
        updatedSpawners, updatedPlayers, interactionDuration = self.executeInteractions(
            [getLastSpawnerVersion(spawnerId) for spawnerId in self.spawnersById.keys()],
            [getLastPlayerVersion(playerId) for playerId in self.playerDictionary.keys()],
            None if restrictedDuration is None else restrictedDuration - movementDuration,
            restrictDurationToPlayer=move.playerId
        )
        updateSpawnersAndPlayers(updatedSpawners, updatedPlayers)

        # Capture spawner if neutral after interaction
        if move.spawnerId is not None:
            updatedSpawner, updatedPlayer = self.tryCaptureNeutralOrOwnAllegedSpawner(getLastSpawnerVersion(move.spawnerId), getLastPlayerVersion(move.playerId), self.frameCount + movementDuration + interactionDuration)
            updateSpawnersAndPlayers([updatedSpawner], [updatedPlayer])

        # Spawn singuities during interaction
        assignNewSpawnees(self.frameCount + movementDuration, self.frameCount + movementDuration + interactionDuration)

        return DiscreteGameState(
            self.currentPlayerId,
            {playerId: getLastPlayerVersion(playerId) for playerId in self.playerDictionary.keys()},
            [getLastSpawnerVersion(spawner.id) for spawner in self.spawners],
            {spawner.id: getLastSpawnerVersion(spawner.id) for spawner in self.spawners},
            self.frameCount + movementDuration + interactionDuration,
            None
        )

    def getSpawnerInteractions(self, spawners: List[DiscreteSpawner], clusters: List[DiscretePlayer]) -> Tuple[Dict[str, List[DiscretePlayer]], Dict[str, str]]:
        spawnerInteractions: Dict[str, List[DiscretePlayer]] = {}
        spawnerInteractedClusters: Dict[str, str] = {}

        def addSpawnerInteraction(spawner: DiscreteSpawner, cluster: DiscretePlayer):
            spawnerInteractedClusters[cluster.id] = spawner.id

            if spawner.id in spawnerInteractions:
                spawnerInteractions[spawner.id].append(cluster)
            else:
                spawnerInteractions[spawner.id] = [cluster]

        for cluster in clusters:
            closestSpawner = spawners[DiscreteGameState.spawnerCollisionDetector.query(cluster.singuitiesMeanPosition.reshape(1, 2))[0]]

            if PhysicsEstimator.areSinguitiesColliding(cluster.singuitiesMeanPosition, closestSpawner.position):
                addSpawnerInteraction(closestSpawner, cluster)

        return spawnerInteractions, spawnerInteractedClusters

    def getClusterInteractions(self, clusters: List[DiscretePlayer]) -> List[List[DiscretePlayer]]:
        interactions: List[List[DiscretePlayer]] = []

        if len(clusters) <= 1:
            return interactions

        clusterCollisionDetector = KDTree([c.singuitiesMeanPosition for c in clusters])

        interactingClusters = set()

        for cluster in clusters:
            if cluster in interactingClusters:
                continue

            [closest, secondClosest] = clusterCollisionDetector.query(cluster.singuitiesMeanPosition.reshape(1, 2), 2)[0]
            if clusters[closest].id != cluster.id:
                closestCluster = clusters[closest]
            else:
                closestCluster = clusters[secondClosest]

            if PhysicsEstimator.areSinguitiesColliding(clusters[secondClosest].singuitiesMeanPosition, cluster.singuitiesMeanPosition) and closestCluster.id not in interactingClusters:
                interactingClusters.update([cluster.id, closestCluster.id])
                interactions.append([cluster, closestCluster])

        return interactions

    def executeInteractions(self, spawners: List[DiscreteSpawner], players: List[DiscretePlayer], restrictedDuration: int, restrictDurationToPlayer: Optional[str] = None) -> Tuple[List[DiscreteSpawner], List[DiscretePlayer], int]:
        updatedSpawners = []
        updatedPlayers = []

        oldSpawnersById: Dict[str, DiscreteSpawner] = {s.id: s for s in spawners}
        spawnerInteractionSubjects, clusterIdToInteractedSpawner = self.getSpawnerInteractions(spawners, players)

        def updateSpawnersAndPlayers(spawnerId: str, spawnerHealthPoints: float, originalPlayers: List[DiscretePlayer], remainingSinguityCounts: List[int]):
            updatedSpawners.append(oldSpawnersById[spawnerId].loseHealthPointsTo(spawnerHealthPoints))

            for originalPlayer, remainingSinguityCount in zip(originalPlayers, remainingSinguityCounts):
                updatedPlayers.append(originalPlayer.fought(remainingSinguityCount))

        def updatePlayers(originalPlayers: List[DiscretePlayer], remainingSinguityCount: List[int]):
            for originalPlayer, remainingSinguityCount in zip(originalPlayers, remainingSinguityCount):
                updatedPlayers.append(originalPlayer.fought(remainingSinguityCount))

        interactionDuration = restrictedDuration

        if restrictDurationToPlayer is not None and restrictDurationToPlayer in clusterIdToInteractedSpawner:
            interactingSpawnerId = clusterIdToInteractedSpawner[restrictDurationToPlayer]
            spawner = oldSpawnersById[interactingSpawnerId]
            spawnerHealthPoints, interactionDuration, remainingCounts = PhysicsEstimator.estimateFightOverSpawner(
                None if spawner.allegence is None else spawner.allegence.playerId,
                spawner.getHealthPoints(),
                restrictedDuration,
                [(cluster.singuityCount, cluster.singuitiesStd, cluster.singuitiesAverageHealth) for cluster in spawnerInteractionSubjects[interactingSpawnerId]]
            )
            updateSpawnersAndPlayers(interactingSpawnerId, spawnerHealthPoints, spawnerInteractionSubjects[interactingSpawnerId], remainingCounts)

        for spawnerId, interactionSubjects in spawnerInteractionSubjects.items():
            spawner = oldSpawnersById[spawnerId]
            spawnerHealthPoints, interactionDuration, remainingCounts = PhysicsEstimator.estimateFightOverSpawner(
                None if spawner.allegence is None else spawner.allegence.playerId,
                spawner.getHealthPoints(),
                restrictedDuration,
                [(cluster.singuityCount, cluster.singuitiesStd, cluster.singuitiesAverageHealth) for cluster in interactionSubjects]
            )
            updateSpawnersAndPlayers(spawnerId, spawnerHealthPoints, interactionSubjects, remainingCounts)

        clusterInteractions = self.getClusterInteractions([cluster for cluster in players if cluster.id not in clusterIdToInteractedSpawner])

        for interactingClusters in clusterInteractions:
            remainingCounts = PhysicsEstimator.estimateVoidFight(
                [(cluster.singuityCount, cluster.singuitiesStd, cluster.singuitiesAverageHealth) for cluster in interactingClusters]
            )
            updatePlayers(interactingClusters, remainingCounts)

        return updatedSpawners, updatedPlayers, interactionDuration

    def tryCaptureNeutralOrOwnAllegedSpawner(self, oldSpawner: DiscreteSpawner, oldPlayer: DiscretePlayer, frameCount: int) -> Tuple[DiscreteSpawner, DiscretePlayer]:
        newSpawner = oldSpawner
        newPlayer = oldPlayer

        if oldSpawner.isAllegedToPlayer(oldPlayer.id) or oldSpawner.allegence is None:
            remainingSinguitiesToClaim = oldSpawner.remainingSinguitiesToCapture(oldPlayer.id)
            newSpawner = oldSpawner.tryClaimedBy(oldPlayer.id, oldPlayer.singuityCount, frameCount)
            newPlayer = oldPlayer.tryClaimFor(remainingSinguitiesToClaim)

        return newSpawner, newPlayer

    def discreteMoveToMove(self, discreteMove: DiscreteMove) -> Move:
        singuityIds = [s.id for s in self.rootGameState.singuities if s.playerId == discreteMove.playerId]

        if discreteMove.spawnerId is not None:
            return Move.fromSpawner(singuityIds, arrays.first(self.rootGameState.spawners, lambda s: s.id == discreteMove.spawnerId), discreteMove.playerId)

        elif discreteMove.position is not None:
            return Move.fromPosition(singuityIds, discreteMove.position)

        else:
            return Move.fromNothing()
