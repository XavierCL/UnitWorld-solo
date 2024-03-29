from __future__ import annotations

from typing import Callable, Dict, List, Optional, Tuple

import numpy as np
from sklearn.neighbors import KDTree

from clientAis.ais.discreteV1.models.DiscretePlayer import DiscretePlayer
from clientAis.ais.discreteV1.models.DiscreteSpawner import DiscreteSpawner
from clientAis.ais.discreteV1.physics.PhysicsEstimator import PhysicsEstimator
from clientAis.ais.discreteV1.Plans.DiscreteMove import DiscreteMove
from clientAis.ais.discreteV1.Plans.Move import Move
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
        playerSpawners: Dict[str, List[DiscreteSpawner]],
        frameCount: int,
        gameState: Optional[GameState]
    ):
        self.currentPlayerId = currentPlayerId
        self.playerDictionary = playerDictionary
        self.spawners = spawners
        self.spawnersById = spawnersById
        self.playerSpawners = playerSpawners
        self.frameCount = frameCount
        self.rootGameState = gameState

    @staticmethod
    def fromGameState(gameState: GameState, currentPlayerId: str) -> DiscreteGameState:
        playerDictionary: Dict[str, DiscretePlayer] = {}
        playerIds: List[str] = []

        DiscreteGameState.spawnerCollisionDetector = KDTree(np.array([s.position for s in gameState.spawners]))

        for playerIndex, player in enumerate(gameState.players):
            discretePlayer = DiscretePlayer.fromGameState(gameState, player.id, currentPlayerId)

            if discretePlayer.singuityCount == 0 and len(
                    [s for s in gameState.spawners if s.allegence is not None and s.allegence.isClaimed and s.allegence.playerId == player.id]
            ) == 0:
                continue

            playerDictionary[player.id] = discretePlayer
            playerIds.append(player.id)

        spawners = [DiscreteSpawner.fromSpawner(s) for s in gameState.spawners]
        spawnersById: Dict[str, DiscreteSpawner] = {s.id: s for s in spawners}
        spawnersByPlayer: Dict[str, List[DiscreteSpawner]] = {p: [s for s in spawners if s.isAllegedToPlayer(p)] for p in playerIds}

        return DiscreteGameState(currentPlayerId, playerDictionary, spawners, spawnersById, spawnersByPlayer, gameState.frameCount, gameState)

    def executeMove(self, move: DiscreteMove, restrictedFrameCount: Optional[int] = None) -> DiscreteGameState:
        return self.preprocessMove(move, restrictedFrameCount)[1]()

    def preprocessMove(self, move: DiscreteMove, restrictedFrameCount: Optional[int] = None) -> Tuple[int, Callable[[], DiscreteGameState]]:
        restrictedDuration = None if (restrictedFrameCount is None) else (restrictedFrameCount - self.frameCount)

        if restrictedFrameCount is not None and restrictedFrameCount <= self.frameCount:
            return 0, lambda: self

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

        def assignNewSpawnees(spawners: List[DiscreteSpawner], frameBefore: int, frameAfter: int):
            if frameAfter > frameBefore:
                spawnersByPlayer = {playerId: [] for playerId in self.playerDictionary.keys()}

                for spawner in spawners:
                    if spawner.isClaimed():
                        spawnersByPlayer[spawner.allegence.playerId].append(spawner)

                for playerId, claimedSpawners in spawnersByPlayer.items():
                    newPlayersById[playerId] = getLastPlayerVersion(playerId).appendNewSpawned(
                        [(spawner.position, spawner.lastFrameClaimed) for spawner in claimedSpawners], frameBefore, frameAfter
                    )

        # Get time until cluster arrives
        targetSpawner = None if move.spawnerId is None else getLastSpawnerVersion(move.spawnerId)
        movementDuration, movedPlayer = getLastPlayerVersion(move.playerId).executeMovement(
            move, move.position if targetSpawner is None else targetSpawner.position, restrictedDuration,
            acceptableSinguityCount=None if targetSpawner is None or targetSpawner.isAllegedToPlayer(
                move.playerId
            ) else targetSpawner.remainingSinguitiesToCapture(move.playerId)
        )

        # Process interactions until player arrives
        updatedSpawners, updatedPlayers, interactionDuration = self.executeInteractions(
            self.spawners, [p for p in self.playerDictionary.values() if p.id != move.playerId], movementDuration
        )

        # Update both interactions and moved player
        updateSpawnersAndPlayers(updatedSpawners, updatedPlayers + [movedPlayer])

        # Take snapshot of spawners to make them spawn after the interaction
        spawnersAfterMovement = [getLastSpawnerVersion(spawner.id) for spawner in self.spawners]

        # Spawn singuities before player fights
        assignNewSpawnees(self.spawners, self.frameCount, self.frameCount + movementDuration)

        # Process main interactions once player is at destination
        updatedSpawners, updatedPlayers, interactionDuration = self.executeInteractions(
            [getLastSpawnerVersion(spawner.id) for spawner in self.spawners],
            [getLastPlayerVersion(playerId) for playerId in self.playerDictionary.keys()],
            PhysicsEstimator.MAXIMUM_INTERACTION_DURATION if restrictedDuration is None else restrictedDuration - movementDuration,
            restrictDurationToPlayer=move.playerId
        )

        def finalizeState():
            updateSpawnersAndPlayers(updatedSpawners, updatedPlayers)

            if targetSpawner is not None:
                updatedTargetSpawner = getLastSpawnerVersion(targetSpawner.id)

                # Capture spawner if neutral after interaction
                if not targetSpawner.isClaimed() and (
                        updatedTargetSpawner.allegence is None or (not updatedTargetSpawner.isClaimed() and updatedTargetSpawner.isAllegedToPlayer(move.playerId))):
                    updatedSpawner, updatedPlayer = self.tryCaptureNeutralOrOwnAllegedSpawner(
                        getLastSpawnerVersion(targetSpawner.id), getLastPlayerVersion(move.playerId), self.frameCount + movementDuration + interactionDuration
                    )
                    updateSpawnersAndPlayers([updatedSpawner], [updatedPlayer])

            # Spawn singuities during interaction
            assignNewSpawnees(spawnersAfterMovement, self.frameCount + movementDuration, self.frameCount + movementDuration + interactionDuration)

            spawners = [getLastSpawnerVersion(spawner.id) for spawner in self.spawners]

            return DiscreteGameState(
                self.currentPlayerId,
                {playerId: getLastPlayerVersion(playerId) for playerId in self.playerDictionary.keys()},
                spawners,
                {spawner.id: spawner for spawner in spawners},
                {p: [s for s in spawners if s.isAllegedToPlayer(p)] for p in self.playerDictionary.keys()},
                self.frameCount + movementDuration + interactionDuration,
                None
            )

        return movementDuration + interactionDuration, finalizeState

    def getSpawnerInteractions(self, spawners: List[DiscreteSpawner], clusters: List[DiscretePlayer]) -> Tuple[Dict[str, List[Tuple[DiscretePlayer, float]]], Dict[str, str]]:
        spawnerInteractions: Dict[str, List[Tuple[DiscretePlayer, float]]] = {}
        spawnerInteractedClusters: Dict[str, str] = {}

        def addSpawnerInteraction(spawner: DiscreteSpawner, cluster: DiscretePlayer, addedDistance2: float):
            spawnerInteractedClusters[cluster.id] = spawner.id

            if spawner.id in spawnerInteractions:
                spawnerInteractions[spawner.id].append((cluster, addedDistance2))
            else:
                spawnerInteractions[spawner.id] = [(cluster, addedDistance2)]

        clusterPositions = np.array([cluster.singuitiesMeanPosition for cluster in clusters])
        closestSpawnerIndices = DiscreteGameState.spawnerCollisionDetector.query(clusterPositions, return_distance=False).reshape(-1)

        for cluster, closestSpawnerIndex in zip(clusters, closestSpawnerIndices):
            closestSpawner = spawners[closestSpawnerIndex]

            isColliding, distance2 = PhysicsEstimator.areSinguitiesColliding(cluster.singuitiesMeanPosition, closestSpawner.position, cluster.singuitiesStd, returnDistance=True)

            if isColliding:
                addSpawnerInteraction(closestSpawner, cluster, distance2)

        return spawnerInteractions, spawnerInteractedClusters

    def getClusterInteractions(self, clusters: List[DiscretePlayer]) -> List[List[Tuple[DiscretePlayer, float]]]:
        interactions: List[List[Tuple[DiscretePlayer, float]]] = []

        if len(clusters) <= 1:
            return interactions

        clusterCollisionDetector = KDTree(np.array([c.singuitiesMeanPosition for c in clusters]))

        interactingClusters = set()

        for cluster in clusters:
            if cluster in interactingClusters:
                continue

            [closest, secondClosest] = clusterCollisionDetector.query(cluster.singuitiesMeanPosition.reshape(1, 2), 2, return_distance=False)[0]
            if clusters[closest].id != cluster.id:
                closestCluster = clusters[closest]
            else:
                closestCluster = clusters[secondClosest]

            isColliding, distance2 = PhysicsEstimator.areSinguitiesColliding(
                clusters[secondClosest].singuitiesMeanPosition, cluster.singuitiesMeanPosition, clusters[secondClosest].singuitiesStd + cluster.singuitiesStd, returnDistance=True
            )

            if isColliding and closestCluster.id not in interactingClusters:
                interactingClusters.update([cluster.id, closestCluster.id])
                interactions.append([(cluster, distance2), (closestCluster, distance2)])

        return interactions

    def executeInteractions(
        self, spawners: List[DiscreteSpawner], players: List[DiscretePlayer], restrictedDuration: Optional[int], restrictDurationToPlayer: Optional[str] = None
    ) -> Tuple[List[DiscreteSpawner], List[DiscretePlayer], int]:
        if len(players) == 0:
            return [], [], 0

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
                [(cluster.id, cluster.singuityCount, cluster.singuitiesStd, cluster.singuitiesAverageHealth, distance2) for (cluster, distance2) in
                 spawnerInteractionSubjects[interactingSpawnerId]]
            )
            updateSpawnersAndPlayers(interactingSpawnerId, spawnerHealthPoints, list(list(zip(*spawnerInteractionSubjects[interactingSpawnerId]))[0]), remainingCounts)
            del spawnerInteractionSubjects[interactingSpawnerId]

        # No interaction duration restriction
        elif restrictDurationToPlayer is not None and restrictedDuration is None:
            return [], [], 0

        for spawnerId, interactionSubjects in spawnerInteractionSubjects.items():
            spawner = oldSpawnersById[spawnerId]
            spawnerHealthPoints, _, remainingCounts = PhysicsEstimator.estimateFightOverSpawner(
                None if spawner.allegence is None else spawner.allegence.playerId,
                spawner.getHealthPoints(),
                interactionDuration,
                [(cluster.id, cluster.singuityCount, cluster.singuitiesStd, cluster.singuitiesAverageHealth, distance2) for (cluster, distance2) in interactionSubjects]
            )
            updateSpawnersAndPlayers(spawnerId, spawnerHealthPoints, list(list(zip(*interactionSubjects))[0]), remainingCounts)

        clusterInteractions = self.getClusterInteractions([cluster for cluster in players if cluster.id not in clusterIdToInteractedSpawner])

        for interactingClusters in clusterInteractions:
            remainingCounts = PhysicsEstimator.estimateVoidFight(
                [(cluster.singuityCount, cluster.singuitiesStd, cluster.singuitiesAverageHealth, distance2) for (cluster, distance2) in interactingClusters]
            )
            updatePlayers(list(list(zip(*interactingClusters))[0]), remainingCounts)

        return updatedSpawners, updatedPlayers, interactionDuration

    def tryCaptureNeutralOrOwnAllegedSpawner(self, oldSpawner: DiscreteSpawner, oldPlayer: DiscretePlayer, frameCount: int) -> Tuple[DiscreteSpawner, DiscretePlayer]:
        newSpawner = oldSpawner
        newPlayer = oldPlayer

        if oldSpawner.isAllegedToPlayer(oldPlayer.id) or oldSpawner.allegence is None:
            remainingSinguitiesToClaim = oldSpawner.remainingSinguitiesToCapture(oldPlayer.id)
            newSpawner = oldSpawner.tryClaimedBy(oldPlayer.id, oldPlayer.singuityCount, frameCount)
            newPlayer = oldPlayer.tryClaimFor(remainingSinguitiesToClaim)

        return newSpawner, newPlayer

    def discreteMoveToMove(self, discreteMove: DiscreteMove) -> List[Move]:
        discretePlayer = self.playerDictionary[discreteMove.playerId]
        ownSinguities = np.array([s for s in self.rootGameState.singuities if s.playerId == discreteMove.playerId], dtype=object)
        singuityIds = np.array([s.id for s in ownSinguities])
        inClusterMask = np.isin(singuityIds, discretePlayer.inCluster)
        outOfCluster = singuityIds[~inClusterMask]

        moves = []

        if discreteMove.spawnerId is not None:
            moves.append(Move.fromSpawner(discretePlayer.inCluster, arrays.first(self.rootGameState.spawners, lambda s: s.id == discreteMove.spawnerId), discreteMove.playerId))

        elif discreteMove.position is not None:
            moves.append(Move.fromPosition(discretePlayer.inCluster, discreteMove.position))

        if len(outOfCluster) > 0 and len(moves) > 0:
            moves.append(Move.fromPosition(list(outOfCluster), moves[-1].position))

        return moves
