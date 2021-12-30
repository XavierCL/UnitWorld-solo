from __future__ import annotations

from typing import List

from clientAis.ais.discreteV1.Plans.DiscreteMove import DiscreteMove

class StateScore:
    def __init__(self, currentPlayerVictory: bool, enemyVictory: bool, frameAchieved: int, occurrences: int, movePath: List[DiscreteMove], path: List[int]):
        self.currentPlayerVictory = currentPlayerVictory
        self.enemyVictory = enemyVictory
        self.frameAchieved = frameAchieved
        self.occurrences = occurrences
        self.movePath = movePath
        self.path = path

    @staticmethod
    def loss(frameAchieved: int) -> StateScore:
        return StateScore(False, True, frameAchieved, 1, [], [])

    @staticmethod
    def win(frameAchieved: int) -> StateScore:
        return StateScore(True, False, frameAchieved, 1, [], [])

    @staticmethod
    def tie(frameAchieved: int) -> StateScore:
        return StateScore(False, False, frameAchieved, 1, [], [])

    def mergeWith(self, otherScore: StateScore):
        return StateScore(self.currentPlayerVictory, self.enemyVictory, self.frameAchieved, self.occurrences + otherScore.occurrences, self.movePath, self.path)

    def getBestScore(self, otherScore: StateScore) -> StateScore:
        if self.isSameAs(otherScore):
            return self.mergeWith(otherScore)

        elif self.isBetterThan(otherScore):
            return self

        return otherScore

    def isSameAs(self, otherScore: StateScore) -> bool:
        return self.currentPlayerVictory == otherScore.currentPlayerVictory and self.enemyVictory == otherScore.enemyVictory and self.frameAchieved == otherScore.frameAchieved

    def isBetterThan(self, otherScore: StateScore) -> bool:
        if self.currentPlayerVictory and not otherScore.currentPlayerVictory:
            return True
        elif not self.currentPlayerVictory and otherScore.currentPlayerVictory:
            return False
        elif not self.enemyVictory and otherScore.enemyVictory:
            return True
        elif self.currentPlayerVictory:
            return self.frameAchieved < otherScore.frameAchieved
        elif self.enemyVictory:
            return self.frameAchieved > otherScore.frameAchieved

        # If its a tie, get it as soon as possible
        return self.frameAchieved < otherScore.frameAchieved

    def isSamePlanAs(self, otherScore: StateScore) -> bool:
        return self.isSameAs(otherScore) and self.occurrences == otherScore.occurrences

    def isPlanBetterThan(self, otherScore: StateScore) -> bool:
        return self.isBetterThan(otherScore) or self.isSameAs(otherScore) and self.occurrences > otherScore.occurrences

    def prependPath(self, move: DiscreteMove, index: int) -> StateScore:
        return StateScore(self.currentPlayerVictory, self.enemyVictory, self.frameAchieved, self.occurrences, [move] + self.movePath, [index] + self.path)
