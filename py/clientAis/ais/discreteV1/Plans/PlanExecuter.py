from typing import Dict, List

from clientAis.ais.discreteV1.models.DiscreteGameState import DiscreteGameState, DiscretePlayer, DiscreteSpawner, DiscreteSpawnerAllegence
from clientAis.ais.discreteV1.Plans.DiscreteMove import DiscreteMove
from clientAis.ais.discreteV1.Move import Move
from clientAis.games.GameState import Spawner
from utils import arrays

class PlanExecuter:
    @staticmethod
    def executePlayerMove(player: DiscretePlayer, move: DiscreteMove, spawner: DiscreteSpawner) -> DiscretePlayer:
        if move.spawnerId is None:
            return DiscretePlayer(player.id, player.singuityCount, move.position, player.singuitiesStd)

        return DiscretePlayer(player.id, max(player.singuityCount - spawner.remainingSinguitiesToCapture(player.id), 0), spawner.position, player.singuitiesStd)

    @staticmethod
    def executeSpawnerMove(spawner: DiscreteSpawner, move: DiscreteMove, player: DiscretePlayer, currentFrame: int) -> DiscreteSpawner:
        if move.spawnerId is not None and move.spawnerId == spawner.id:
            remainingSinguities = spawner.remainingSinguitiesToCapture(player.id)
            if player.singuityCount >= remainingSinguities:
                return DiscreteSpawner(spawner.id, spawner.position, DiscreteSpawnerAllegence(True, player.id, Spawner.MAX_HEALTH_POINTS), currentFrame + move.totalFrameCount)
            else:
                return DiscreteSpawner(
                    spawner.id,
                    spawner.position,
                    DiscreteSpawnerAllegence(
                        False,
                        player.id,
                        Spawner.MAX_HEALTH_POINTS * (Spawner.REQUIRED_CAPTURING_SINGUITIES - remainingSinguities + player.singuityCount) / Spawner.REQUIRED_CAPTURING_SINGUITIES
                    ),
                    spawner.lastFrameClaimed
                )

        return spawner

    @staticmethod
    def executeGameStatePlan(gameState: DiscreteGameState, plan: List[DiscreteMove]) -> DiscreteGameState:
        newPlayersById: Dict[str, DiscretePlayer] = {}
        newSpawnersById: Dict[str, DiscreteSpawner] = {}

        def getLastPlayerVersion(playerId: str) -> DiscretePlayer:
            if playerId in newPlayersById:
                return newPlayersById[playerId]

            return gameState.playerDictionary[playerId]

        def getLastSpawnerVersion(spawnerId: str) -> DiscreteSpawner:
            if spawnerId in newSpawnersById:
                return newSpawnersById[spawnerId]

            return gameState.spawnersById[spawnerId]

        for move in plan:
            oldSpawner = None
            oldPlayer = getLastPlayerVersion(move.playerId)

            if move.spawnerId is not None:
                oldSpawner = getLastSpawnerVersion(move.spawnerId)
                newSpawnersById[move.spawnerId] = PlanExecuter.executeSpawnerMove(oldSpawner, move, oldPlayer, gameState.frameCount)

            newPlayersById[move.playerId] = PlanExecuter.executePlayerMove(oldPlayer, move, oldSpawner)

        return DiscreteGameState(
            gameState.currentPlayerId,
            {playerId: getLastPlayerVersion(playerId) for playerId in gameState.playerIds},
            gameState.playerIds,
            gameState.currentPlayerIndex,
            [getLastSpawnerVersion(spawner.id) for spawner in gameState.spawners],
            {spawner.id: getLastSpawnerVersion(spawner.id) for spawner in gameState.spawners},
            max([move.totalFrameCount for move in plan]),
            None
        )

    @staticmethod
    def discreteMoveToMove(gameState: DiscreteGameState, discreteMove: DiscreteMove) -> Move:
        singuityIds = [s.id for s in gameState.rootGameState.singuities if s.playerId == discreteMove.playerId]

        if discreteMove.spawnerId is not None:
            return Move.fromSpawner(singuityIds, arrays.first(gameState.rootGameState.spawners, lambda s: s.id == discreteMove.spawnerId), discreteMove.playerId)

        return Move.fromPosition(singuityIds, discreteMove.position)