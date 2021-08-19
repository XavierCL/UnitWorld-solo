import json
import os
import random
import shutil
from typing import Any, Dict, List, Union

import numpy as np

from utils import arrays

class MapHandler:
    def __init__(self, lastGamePath: str, archivedGamesPath: str):
        self.lastGamePath = lastGamePath
        self.archivedGamesPath = archivedGamesPath

        self.generatedMap = None

    def archiveLastMap(self):
        if len([True for file in os.listdir(self.lastGamePath) if file == "config.json"]) == 1:
            archivedGameIds = [
                int(fileName[4:])
                for fileName
                in os.listdir(self.archivedGamesPath)
                if len(fileName) >= 4 and fileName[4:].isdigit()
            ]

            archivedGameId = 0 if len(archivedGameIds) == 0 else np.max(archivedGameIds) + 1
            archivedGamePath = os.path.join(self.archivedGamesPath, f"game{archivedGameId}")
            os.mkdir(archivedGamePath)
            shutil.move(os.path.join(self.lastGamePath, "config.json"), os.path.join(archivedGamePath, "config.json"))

    def generateRandomMap(self, mapSizeInGameUnits=5000.0, discreteMapSize=41, forcedSpawners=3, playerCount=2, expectedRandomSpawners=5, mapBordersInGameUnit=500) -> Dict[str, Any]:
        # Creating new map
        self.generatedMap = {
            "server ip": "127.0.0.1",
            "server port": "52124",
            "move-zone-width-ratio": 0.03,
            "translation-pixel-per-frame": 30.0,
            "scroll-ratio-per-tick": 0.2,
            "ai-name": "spearAi",
            "world-absolute-width": mapSizeInGameUnits,
            "world-absolute-height": mapSizeInGameUnits,
            "fog-of-war": False
        }

        asymmetricSpawnerConfig = np.random.random((discreteMapSize, discreteMapSize)) < expectedRandomSpawners / discreteMapSize ** 2

        # Forcing at least a few spawners
        asymmetricSpawnerConfig[np.random.randint(0, discreteMapSize, forcedSpawners), np.random.randint(0, discreteMapSize, forcedSpawners)] = True

        # Making sure the map is fair by forcing symmetry
        symmetryFunction = random.choice(
            [
                # lambda asymmetric: np.any([asymmetric, asymmetric[::-1]], axis=0),  # x axis
                # lambda asymmetric: np.any([asymmetric, asymmetric[:, ::-1]], axis=0),  # y axis
                # lambda asymmetric: np.any([asymmetric, asymmetric.T], axis=0),  # 1, 1 diagonal
                # lambda asymmetric: np.any([asymmetric, asymmetric[::-1].T[::-1]], axis=0),  # 1, -1 diagonal
                lambda asymmetric: arrays.assign(asymmetric, tuple((np.array([discreteMapSize - 1, discreteMapSize - 1]) - np.array(np.nonzero(asymmetric)).T).T), True),  # 180 degree rotation
            ]
        )

        symmetricSpawnerConfig = symmetryFunction(asymmetricSpawnerConfig)

        # Generating home spawners
        possibleHomes = []

        for squareIndex in range(0, np.prod(symmetricSpawnerConfig.shape)):
            if np.count_nonzero(
                    symmetryFunction(arrays.assign(np.zeros((discreteMapSize, discreteMapSize), dtype=bool).reshape(-1), squareIndex, True).reshape(discreteMapSize, -1))
            ) == playerCount:
                possibleHomes.append(squareIndex)

        homeSquareIndex = np.random.choice(possibleHomes)
        homeSpawnerConfig = symmetryFunction(
            arrays.assign(np.zeros((discreteMapSize, discreteMapSize), dtype=bool).reshape(-1), homeSquareIndex, True).reshape(discreteMapSize, -1)
        )

        # Removing neutral spawner from home location
        symmetricSpawnerConfig[homeSpawnerConfig] = False

        # Generating final spawner array
        viewPointInGameUnit = mapSizeInGameUnits - mapBordersInGameUnit
        gamePositionOffset = (mapSizeInGameUnits - viewPointInGameUnit * (discreteMapSize - 1) / discreteMapSize) / 2

        def discretePositionToGamePosition(pos: int) -> float:
            return viewPointInGameUnit * pos / discreteMapSize + gamePositionOffset

        spawnersConfig: List[List[Dict[str, Union[float, int]]]] = [
            [{"x": discretePositionToGamePosition(x), "y": discretePositionToGamePosition(y)} for x, y in zip(*np.nonzero(symmetricSpawnerConfig))]
        ]

        spawnersConfig = spawnersConfig + [[{"x": discretePositionToGamePosition(x), "y": discretePositionToGamePosition(y), "singuities": 150}] for x, y in
                                           zip(*np.nonzero(homeSpawnerConfig))]

        self.generatedMap["spawners"] = spawnersConfig

        return self.generatedMap

    def saveMapToFile(self):
        with open(os.path.join(self.lastGamePath, 'config.json'), 'w', encoding='utf-8') as fileHandle:
            json.dump(self.generatedMap, fileHandle, ensure_ascii=False, indent=4)
