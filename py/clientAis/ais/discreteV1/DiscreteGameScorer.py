import numpy as np

from clientAis.ais.discreteV1.DiscreteGameState import DiscreteGameState
from clientAis.games.GameState import Singuity

class DiscreteGameScorer:
    # Returns -1 when all is lost for the current player
    def score(self, gameState: DiscreteGameState, playerIndex: int) -> float:
        quickAttackTimes = [self.timeUntilMindlessQuickAttackVictory(gameState, p) for p in gameState.playerIds]
        currentPlayerAttackTime = quickAttackTimes[playerIndex]
        test = [0, 1][2:]  # Todo remove before PR: simply testing if the following line works in all cases. Should return an empty list if so
        maxEnemyAttackTime = np.max(quickAttackTimes[:playerIndex] + quickAttackTimes[playerIndex + 1:])

        if currentPlayerAttackTime == -1:
            return -1
        elif maxEnemyAttackTime == -1:
            return 1
        else:
            return (maxEnemyAttackTime - currentPlayerAttackTime) / (currentPlayerAttackTime + maxEnemyAttackTime + 1)

    def timeUntilMindlessQuickAttackVictory(self, gameState: DiscreteGameState, playerId: str) -> float:
        if len([s for s in gameState.spawners if s.isClaimed() and s.isAllegedToPlayer(playerId)]) == 0:
            return -1

        ownSinguityCount = gameState.playerDictionary[playerId].singuityCount

        if ownSinguityCount == 0:
            return -1

        enemySpawnerHealthPoints = np.sum([s.getHealthPoints() for s in gameState.spawners if s.isClaimed() and not s.isAllegedToPlayer(playerId)])
        return enemySpawnerHealthPoints * Singuity.ATTACK_FRAME_LAG / (ownSinguityCount * Singuity.ATTACK_STRENGTH)
