from typing import Tuple

from clientAis.ais.discreteV1.DiscreteGameScorer import DiscreteGameScorer
from clientAis.ais.discreteV1.DiscreteGameState import DiscreteGameState
from clientAis.ais.discreteV1.DiscretePlanGenerator import DiscretePlanGenerator
from clientAis.ais.discreteV1.PlanExecuter import PlanExecuter

class DiscreteGameSearcher:
    def __init__(self, moveGenerator: DiscretePlanGenerator, gameScorer: DiscreteGameScorer, depth: int):
        self.planGenerator = moveGenerator
        self.gameScorer = gameScorer
        self.depth = depth - 1

    def getBestOwnPlan(self, currentGameState: DiscreteGameState):
        plans = self.planGenerator.generatePlans(currentGameState, currentGameState.currentPlayerId)

        bestPlan = plans[0]
        outcomeGameState = PlanExecuter.executeGameStatePlan(currentGameState, bestPlan)
        bestScore, bestFrameCount = self.getRecursiveBestPlanScore(outcomeGameState, self.depth)
        bestFrameCount += max([move.totalFrameCount for move in bestPlan])

        for plan in plans[1:]:
            outcomeGameState = PlanExecuter.executeGameStatePlan(currentGameState, plan)
            planScore, planFrameCount = self.getRecursiveBestPlanScore(outcomeGameState, self.depth)
            planFrameCount += max([move.totalFrameCount for move in plan])

            if planScore > bestScore or planScore == bestScore and planFrameCount < bestFrameCount:
                bestPlan = plan
                bestScore = planScore
                bestFrameCount = planFrameCount

        return [PlanExecuter.discreteMoveToMove(currentGameState, discreteMove) for discreteMove in bestPlan if discreteMove.playerId == currentGameState.currentPlayerId]

    # Returns the score [-1, 1] and frame count to get to that score offset by the game state frame count
    def getRecursiveBestPlanScore(self, gameState: DiscreteGameState, depth: int) -> Tuple[float, int]:
        if gameState.hasWon(gameState.currentPlayerId):
            return 1, 0

        elif depth == 0:
            return self.gameScorer.score(gameState, gameState.currentPlayerIndex), 0

        plans = self.planGenerator.generatePlans(gameState, gameState.currentPlayerId)
        outcomeGameState = PlanExecuter.executeGameStatePlan(gameState, plans[0])
        bestScore, bestFrameCount = self.getRecursiveBestPlanScore(outcomeGameState, depth - 1)
        bestFrameCount += max([move.totalFrameCount for move in plans[0]])

        for plan in plans[1:]:
            outcomeGameState = PlanExecuter.executeGameStatePlan(gameState, plan)
            score, frameCount = self.getRecursiveBestPlanScore(outcomeGameState, depth - 1)
            frameCount += max([move.totalFrameCount for move in plans[0]])

            if score > bestScore or score == bestScore and frameCount < bestFrameCount:
                bestScore = score
                bestFrameCount = frameCount

        return bestScore, bestFrameCount