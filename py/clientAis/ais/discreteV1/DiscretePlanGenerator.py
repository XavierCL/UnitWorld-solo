from typing import List, Tuple

from clientAis.ais.discreteV1.DiscreteGameState import DiscreteGameState
from clientAis.ais.discreteV1.DiscreteMove import DiscreteMove

class DiscretePlanGenerator:
    def generatePlans(self, gameState: DiscreteGameState, playerId: str) -> List[List[DiscreteMove]]:
        moves = []
        for spawner in gameState.spawners:
            moves.append(DiscreteMove.fromSpawner(playerId, spawner.id))

        for _, discretePlayer in gameState.playerDictionary.items():
            moves.append(DiscreteMove.fromPosition(playerId, discretePlayer.singuitiesMeanPosition))

        return [[move] for move in moves]
