from __future__ import annotations

import random
import time
from typing import List, Optional

import numpy as np

from clientAis.ais.discreteV1.DiscreteGameScorer import DiscreteGameScorer
from clientAis.ais.discreteV1.models.DiscreteGameState import DiscreteGameState
from clientAis.ais.discreteV1.Plans.DiscreteMove import DiscreteMove
from clientAis.ais.discreteV1.Plans.DiscreteMoveGenerator import DiscreteMoveGenerator

class StateScore:
    def __init__(self, score: float, frameAchieved: int, occurrences: int, path: List[int]):
        self.score = score
        self.frameAchieved = frameAchieved
        self.occurrences = occurrences
        self.path = path

    def getBestScore(self, otherScore: StateScore) -> StateScore:
        if self.isSameAs(otherScore):
            return StateScore(self.score, self.frameAchieved, self.occurrences + otherScore.occurrences, self.path)

        elif self.isBetterThan(otherScore):
            return self

        return otherScore

    def isSameAs(self, otherScore: StateScore) -> bool:
        return self.score == otherScore.score and self.frameAchieved == otherScore.frameAchieved

    def isBetterThan(self, otherScore: StateScore) -> bool:
        return self.score > otherScore.score or self.score == otherScore.score and self.frameAchieved < otherScore.frameAchieved

    def isSamePlanAs(self, otherScore: StateScore) -> bool:
        return self.isSameAs(otherScore) and self.occurrences == otherScore.occurrences

    def isPlanBetterThan(self, otherScore: StateScore) -> bool:
        return self.isBetterThan(otherScore) or self.isSameAs(otherScore) and self.occurrences > otherScore.occurrences

    def prependPath(self, index: int) -> StateScore:
        return StateScore(self.score, self.frameAchieved, self.occurrences, [index] + self.path)

class DiscreteGameNode:
    INTEGRAL_DISCOUNT_RATIO = 0.5

    def __init__(self, gameState: DiscreteGameState, previousPlan: Optional[DiscreteMove], parentGameState: Optional[DiscreteGameState], depth: int = 0):
        self.gameState = gameState
        self.previousPlan = previousPlan
        self.parentGameState = parentGameState
        self.children: List[DiscreteGameNode] = []
        self.depth = depth

    def getBestPlan(self) -> List[DiscreteMove]:
        if len(self.children) == 0:
            return DiscreteMove.fromNothing()

        currentNodeScore = DiscreteGameScorer.score(self.gameState, self.gameState.currentPlayerId)
        bestScore = self.children[0].getBestScore(self.gameState.currentPlayerId, self.gameState.frameCount, currentNodeScore)
        bestPlans = [self.children[0].previousPlan]
        bestIndex = 0

        for index, child in enumerate(self.children[1:]):
            currentScore = child.getBestScore(self.gameState.currentPlayerId, self.gameState.frameCount, currentNodeScore)

            if bestScore.isSamePlanAs(currentScore):
                bestPlans.append(child.previousPlan)

            elif currentScore.isPlanBetterThan(bestScore):
                bestScore = currentScore
                bestPlans = [child.previousPlan]
                bestIndex = index + 1

        bestScore = bestScore.prependPath(bestIndex)
        return [random.choice(bestPlans)]

    def getBestScore(self, playerId: str, rootFrameCount: int, parentScore: float, integralDiscount: float = 0) -> StateScore:
        currentNodeScore = DiscreteGameScorer.score(self.gameState, playerId)

        # Add average siblings also, so that a plan with more good scores performs better. Removes the need for the "occurrence" variable.
        integralDiscount = integralDiscount + parentScore * (self.gameState.frameCount - self.parentGameState.frameCount)

        if len(self.children) == 0:
            return StateScore((1 - DiscreteGameNode.INTEGRAL_DISCOUNT_RATIO) * currentNodeScore + integralDiscount * DiscreteGameNode.INTEGRAL_DISCOUNT_RATIO / (self.gameState.frameCount - rootFrameCount), self.gameState.frameCount, 1, [])

        bestScore = self.children[0].getBestScore(playerId, rootFrameCount, currentNodeScore, integralDiscount)
        bestIndex = 0

        for index, child in enumerate(self.children[1:]):
            currentScore = child.getBestScore(playerId, rootFrameCount, currentNodeScore, integralDiscount)
            bestScore = bestScore.getBestScore(currentScore)
            bestIndex = index + 1 if bestScore is currentScore else bestIndex

        return bestScore.prependPath(bestIndex)

    def developChildren(self) -> List[DiscreteGameNode]:
        nextSteps = DiscreteMoveGenerator.executeStep(self.gameState, self.gameState.currentPlayerId)
        self.children = [DiscreteGameNode(nextGameState, nextMove, self.gameState, self.depth + 1) for nextMove, nextGameState in nextSteps]
        return self.children

    def restrictDuration(self, frameCount: int) -> DiscreteGameNode:
        self.gameState = self.parentGameState.executeMove(self.previousPlan, restrictedFrameCount=frameCount)
        return self

class DiscreteGameSearcher:
    def __init__(self, allottedGenerationTimeSeconds=0.3, maxDepth: int = None):
        self.allottedGenerationTimeSeconds = allottedGenerationTimeSeconds
        self.maxDepth = maxDepth

    def getBestOwnPlan(self, gameState: DiscreteGameState):
        developedGameStates = 0
        rootGameNode = DiscreteGameNode(gameState, None, None)

        frameCounts: List[int] = [gameState.frameCount]
        gameLeaves: List[DiscreteGameNode] = [rootGameNode]
        orphanGameLeaves: List[DiscreteGameNode] = []

        startTime = time.time()

        while time.time() - startTime < self.allottedGenerationTimeSeconds and len(frameCounts) > 0:
            developedGameStates += 1
            frameCounts.pop(0)
            gameNode = gameLeaves.pop(0)

            if self.maxDepth is not None and gameNode.depth == self.maxDepth - 1:
                orphanGameLeaves.extend(gameNode.developChildren())
                continue

            movedGameNodes = np.array(gameNode.developChildren()[:], dtype=object)
            movedFrameCounts = np.array([childNode.gameState.frameCount for childNode in movedGameNodes])

            reverseSortedMovedFrameCountIndices = np.argsort(movedFrameCounts)[::-1]

            movedGameNodes = movedGameNodes[reverseSortedMovedFrameCountIndices]
            movedFrameCounts = movedFrameCounts[reverseSortedMovedFrameCountIndices]

            frameCountIndices = np.searchsorted(frameCounts, movedFrameCounts)

            for childIndex in range(len(movedGameNodes)):
                frameCounts.insert(frameCountIndices[childIndex], movedFrameCounts[childIndex])
                gameLeaves.insert(frameCountIndices[childIndex], movedGameNodes[childIndex])

        smallestFrameCount = min(frameCounts[0:1] + [g.gameState.frameCount for g in orphanGameLeaves])

        for gameLeaf in gameLeaves + orphanGameLeaves:
            gameLeaf.restrictDuration(smallestFrameCount + 0.001)

        print(f"Developed game nodes: {developedGameStates}")

        return [rootGameNode.gameState.discreteMoveToMove(discreteMove) for discreteMove in rootGameNode.getBestPlan()]
