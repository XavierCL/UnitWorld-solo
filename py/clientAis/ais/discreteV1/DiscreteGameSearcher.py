from typing import Tuple

from clientAis.ais.discreteV1.DiscreteGameScorer import DiscreteGameScorer
from clientAis.ais.discreteV1.models.DiscreteGameState import DiscreteGameState
from clientAis.ais.discreteV1.Plans.DiscretePlanGenerator import DiscretePlanGenerator

class DiscreteGameSearcher:
    def __init__(self, moveGenerator: DiscretePlanGenerator, gameScorer: DiscreteGameScorer, depth: int):
        self.planGenerator = moveGenerator
        self.gameScorer = gameScorer
        self.depth = depth - 1

    def getBestOwnPlan(self, gameState: DiscreteGameState):
        plans = self.planGenerator.generatePlans(gameState, gameState.currentPlayerId)

        bestPlan = plans[0]
        outcomeGameState = gameState.executePlan(bestPlan)
        bestScore, bestFrameCount = self.getRecursiveBestPlanScore(outcomeGameState, self.depth)

        for plan in plans[1:]:
            outcomeGameState = gameState.executePlan(plan)
            planScore, planFrameCount = self.getRecursiveBestPlanScore(outcomeGameState, self.depth)

            if planScore > bestScore or planScore == bestScore and planFrameCount < bestFrameCount:
                bestPlan = plan
                bestScore = planScore
                bestFrameCount = planFrameCount

        return [gameState.discreteMoveToMove(discreteMove) for discreteMove in bestPlan if discreteMove.playerId == gameState.currentPlayerId]

    # Returns the score [-1, 1] and frame count at the best scored game state
    def getRecursiveBestPlanScore(self, gameState: DiscreteGameState, depth: int) -> Tuple[float, int]:
        if gameState.hasWon(gameState.currentPlayerId):
            return 1, gameState.frameCount

        elif depth == 0:
            return self.gameScorer.score(gameState, gameState.currentPlayerIndex), gameState.frameCount

        plans = self.planGenerator.generatePlans(gameState, gameState.currentPlayerId)
        outcomeGameState = gameState.executePlan(plans[0])
        bestScore, bestFrameCount = self.getRecursiveBestPlanScore(outcomeGameState, depth - 1)

        for plan in plans[1:]:
            outcomeGameState = gameState.executePlan(plan)
            score, frameCount = self.getRecursiveBestPlanScore(outcomeGameState, depth - 1)

            if score > bestScore or score == bestScore and frameCount < bestFrameCount:
                bestScore = score
                bestFrameCount = frameCount

        return bestScore, bestFrameCount
