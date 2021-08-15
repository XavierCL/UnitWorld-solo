from __future__ import annotations

import random
import time
from typing import List, Optional

import numpy as np

from clientAis.ais.discreteV1.DiscreteGameScorer import DiscreteGameScorer
from clientAis.ais.discreteV1.models.DiscreteGameState import DiscreteGameState
from clientAis.ais.discreteV1.Plans.DiscreteMove import DiscreteMove
from clientAis.ais.discreteV1.Plans.DiscretePlanGenerator import DiscretePlanGenerator

class StateScore:
    def __init__(self, score: float, frameAchieved: int, occurrences: int):
        self.score = score
        self.frameAchieved = frameAchieved
        self.occurrences = occurrences

    def getBestScore(self, otherScore: StateScore) -> StateScore:
        if self.isSameAs(otherScore):
            return StateScore(self.score, self.frameAchieved, self.occurrences + otherScore.occurrences)

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

class DiscreteGameNode:
    def __init__(self, gameState: DiscreteGameState, previousPlan: Optional[List[DiscreteMove]], parentGameState: Optional[DiscreteGameState]):
        self.gameState = gameState
        self.previousPlan = previousPlan
        self.parentGameState = parentGameState
        self.children: List[DiscreteGameNode] = []

    def getBestPlan(self) -> List[DiscreteMove]:
        if len(self.children) == 0:
            return DiscreteMove.fromNothing()

        bestScore = self.children[0].getBestScore(self.gameState.currentPlayerIndex)
        bestPlans = [self.children[0].previousPlan]

        for child in self.children[1:]:
            currentScore = child.getBestScore(self.gameState.currentPlayerIndex)

            if bestScore.isSamePlanAs(currentScore):
                bestPlans.append(child.previousPlan)

            elif currentScore.isPlanBetterThan(bestScore):
                bestScore = currentScore
                bestPlans = [child.previousPlan]

        return random.choice(bestPlans)

    def getBestScore(self, playerIndex: int) -> StateScore:
        if len(self.children) == 0:
            return StateScore(DiscreteGameScorer.score(self.gameState, playerIndex), self.gameState.frameCount, 1)

        bestScore = self.children[0].getBestScore(playerIndex)

        for child in self.children[1:]:
            bestScore = bestScore.getBestScore(child.getBestScore(playerIndex))

        return bestScore

    def developChildren(self) -> List[DiscreteGameNode]:
        plans = DiscretePlanGenerator.generatePlans(self.gameState, self.gameState.currentPlayerId)
        self.children = [DiscreteGameNode(self.gameState.executePlan(plan), plan, self.gameState) for plan in plans]
        return self.children

    def restrictDuration(self, frameCount: int) -> DiscreteGameNode:
        self.gameState = self.parentGameState.executePlan(self.previousPlan, restrictedFrameCount=frameCount)
        return self

class DiscreteGameSearcher:
    def __init__(self, allottedGenerationTimeSeconds=0.3):
        self.allottedGenerationTimeSeconds = allottedGenerationTimeSeconds

    def getBestOwnPlan(self, gameState: DiscreteGameState):
        rootGameNode = DiscreteGameNode(gameState, None, None)

        frameCounts: List[int] = [gameState.frameCount]
        gameLeaves: List[DiscreteGameNode] = [rootGameNode]

        startTime = time.time()

        while time.time() - startTime < self.allottedGenerationTimeSeconds:
            frameCounts.pop(0)
            gameNode = gameLeaves.pop(0)

            movedGameNodes = np.array(gameNode.developChildren()[:], dtype=object)
            movedFrameCounts = np.array([g.frameCount for g in movedGameNodes])

            reverseSortedMovedFrameCountIndices = np.argsort(movedFrameCounts)[::-1]

            movedGameNodes = movedGameNodes[reverseSortedMovedFrameCountIndices]
            movedFrameCounts = movedFrameCounts[reverseSortedMovedFrameCountIndices]

            frameCountIndices = np.searchsorted(frameCounts, movedFrameCounts)

            for childIndex in range(len(movedGameNodes)):
                frameCounts.insert(movedFrameCounts[childIndex], frameCountIndices[childIndex])
                gameLeaves.insert(movedGameNodes[childIndex], movedGameNodes[childIndex])

        smallestFrameCount = frameCounts[0]

        for gameLeaf in gameLeaves:
            gameLeaf.restrictDuration(smallestFrameCount)

        return rootGameNode.getBestPlan()
