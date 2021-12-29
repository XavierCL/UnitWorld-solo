from __future__ import annotations

import random
import time
from typing import List, Optional

import numpy as np

from clientAis.ais.discreteV1.scores.DiscreteGameScorer import DiscreteGameScorer
from clientAis.ais.discreteV1.models.DiscreteGameState import DiscreteGameState
from clientAis.ais.discreteV1.Plans.DiscreteMove import DiscreteMove
from clientAis.ais.discreteV1.Plans.DiscreteMoveGenerator import DiscreteMoveGenerator
from clientAis.ais.discreteV1.searches.StateScore import StateScore

class DiscreteGameNode:
    INTEGRAL_DISCOUNT_RATIO = 0.1

    def __init__(self, gameState: DiscreteGameState, previousPlan: Optional[DiscreteMove], parentGameState: Optional[DiscreteGameState], depth: int = 0):
        self.gameState = gameState
        self.previousPlan = previousPlan
        self.parentGameState = parentGameState
        self.children: List[DiscreteGameNode] = []
        self.depth = depth

    def getBestPlan(self) -> DiscreteMove:
        if len(self.children) == 0:
            return DiscreteMove.fromNothing()

        bestScore = self.children[0].getBestScore(self.gameState.currentPlayerId, self.gameState.frameCount)
        bestIndices = [0]

        for index, child in enumerate(self.children[1:]):
            currentScore = child.getBestScore(self.gameState.currentPlayerId, self.gameState.frameCount)

            if bestScore.isSamePlanAs(currentScore):
                bestIndices.append(index + 1)

            elif currentScore.isPlanBetterThan(bestScore):
                bestScore = currentScore
                bestIndices = [index + 1]

        chosenIndex = random.choice(bestIndices)
        bestScore = bestScore.prependPath(self.children[chosenIndex].previousPlan, chosenIndex)
        print([move.debugName for move in bestScore.movePath])

        return self.children[chosenIndex].previousPlan

    def getBestScore(self, playerId: str, rootFrameCount: int) -> StateScore:
        if len(self.children) == 0:
            return DiscreteGameScorer.score(self.gameState, playerId)

        bestScore = self.children[0].getBestScore(playerId, rootFrameCount)
        bestIndex = 0

        for index, child in enumerate(self.children[1:]):
            currentScore = child.getBestScore(playerId, rootFrameCount)
            bestScore = bestScore.getBestScore(currentScore)
            bestIndex = index + 1 if bestScore is currentScore else bestIndex

        return bestScore.prependPath(self.children[bestIndex].previousPlan, bestIndex)

    def developChildren(self) -> List[DiscreteGameNode]:
        nextSteps = DiscreteMoveGenerator.executeStep(self.gameState, self.gameState.currentPlayerId)
        self.children = [DiscreteGameNode(nextGameState, nextMove, self.gameState, self.depth + 1) for nextMove, nextGameState in nextSteps]
        return self.children

    def restrictDuration(self, frameCount: int) -> DiscreteGameNode:
        self.gameState = self.parentGameState.executeMove(self.previousPlan, restrictedFrameCount=frameCount)
        return self

class DiscreteGameSearcher:
    def __init__(self, allottedGenerationTimeSeconds: Optional[float] = None, maxDepth: Optional[int] = None):
        self.allottedGenerationTimeSeconds = allottedGenerationTimeSeconds
        self.maxDepth = maxDepth

    def getBestOwnPlan(self, gameState: DiscreteGameState):
        developedGameStates = 0
        reachedGameStates = 1
        rootGameNode = DiscreteGameNode(gameState, None, None)

        frameCounts: List[int] = [gameState.frameCount]
        gameLeaves: List[DiscreteGameNode] = [rootGameNode]
        orphanGameLeaves: List[DiscreteGameNode] = []

        startTime = time.time()

        def shouldContinueExploration() -> bool:
            if len(frameCounts) <= 0:
                return False

            if self.allottedGenerationTimeSeconds is None:
                return True

            currentDuration = time.time() - startTime

            estimatedPostExplorationDuration = (len(gameLeaves) + len(orphanGameLeaves)) * (time.time() - startTime) / (reachedGameStates + 1)

            return currentDuration + estimatedPostExplorationDuration < self.allottedGenerationTimeSeconds

        while shouldContinueExploration():
            frameCounts.pop(0)
            gameNode = gameLeaves.pop(0)

            developedChildren = gameNode.developChildren()
            reachedGameStates += len(developedChildren)
            developedGameStates += 1

            if self.maxDepth is not None and gameNode.depth == self.maxDepth - 1:
                orphanGameLeaves.extend(developedChildren)
                continue

            movedGameNodes = np.array(developedChildren[:], dtype=object)
            movedFrameCounts = np.array([childNode.gameState.frameCount for childNode in movedGameNodes])

            reverseSortedMovedFrameCountIndices = np.argsort(movedFrameCounts)[::-1]

            movedGameNodes = movedGameNodes[reverseSortedMovedFrameCountIndices]
            movedFrameCounts = movedFrameCounts[reverseSortedMovedFrameCountIndices]

            frameCountIndices = np.searchsorted(frameCounts, movedFrameCounts)

            for childIndex in range(len(movedGameNodes)):
                frameCounts.insert(frameCountIndices[childIndex], movedFrameCounts[childIndex])
                gameLeaves.insert(frameCountIndices[childIndex], movedGameNodes[childIndex])

        # smallestFrameCount = min(frameCounts[0:1] + [g.gameState.frameCount for g in orphanGameLeaves])

        allLeaves = gameLeaves + orphanGameLeaves

        for gameLeaf in allLeaves:
            # Must restrict to more than smallest frame count in case the smallest developed move if the same as the smallest undeveloped move,
            # in which case the smallest undeveloped must reach the same result at least
            # Sometimes this metric does not make sense, as with max depth = 1, where only the smallest frame move will be fully computed.
            # gameLeaf.restrictDuration(smallestFrameCount + 0.001)
            pass

        print(f"Developed: {developedGameStates}, Reached: {reachedGameStates}, Leaves: {len(allLeaves)}")

        return rootGameNode.gameState.discreteMoveToMove(rootGameNode.getBestPlan())
