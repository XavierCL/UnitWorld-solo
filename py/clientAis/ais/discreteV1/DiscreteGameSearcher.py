from clientAis.ais.discreteV1.DiscreteGameScorer import DiscreteGameScorer
from clientAis.ais.discreteV1.DiscreteGameState import DiscreteGameState
from clientAis.ais.discreteV1.DiscretePlanGenerator import DiscretePlanGenerator

class DiscreteGameSearcher:
    def __init__(self, moveGenerator: DiscretePlanGenerator, gameScorer: DiscreteGameScorer):
        self.planGenerator = moveGenerator
        self.gameScorer = gameScorer

    def getBestOwnPlan(self, currentGameState: DiscreteGameState):
        plans = self.planGenerator.generatePlans(currentGameState, currentGameState.currentPlayerId)

        bestPlan = plans[0]
        bestScore = self.gameScorer.score(currentGameState.executePlan(bestPlan), currentGameState.currentPlayerIndex)

        for plan in plans[1:]:
            currentScore = self.gameScorer.score(currentGameState.executePlan(plan), currentGameState.currentPlayerIndex)
            if bestScore > currentScore:
                bestPlan = plan

        return [currentGameState.discreteMoveToMove(discreteMove) for discreteMove in bestPlan if discreteMove.playerId == currentGameState.currentPlayerId]
