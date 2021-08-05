import os
import shutil
import random
from typing import Dict, List, Union

import numpy as np

from utils import arrays

gameExecutablesPath = "../../cpp/x64/Release/"
pythonExecutablePath = "../clientAis/main.py"
archivedGamesPath = "./games/archived/"
lastGamePath = "./games/latest/"

# Clean up last run if it exists
if len([True for file in os.listdir(lastGamePath) if file == "config.json"]) == 1:
    archivedGameIds = [
        fileName[4:]
        for fileName
        in os.listdir(archivedGamesPath)
        if len(fileName) >= 4 and fileName[4:].isdigit()
    ]

    archivedGameId = 0 if len(archivedGameIds) == 0 else np.max(archivedGameIds) + 1
    archivedGamePath = os.path.join(archivedGamesPath, f"game{archivedGameId}")
    os.mkdir(archivedGamePath)
    shutil.move(os.path.join(lastGamePath, "config.json"), os.path.join(archivedGamePath, "config.json"))

mapSizeInGameUnits = 5000.0
discreteMapSize = 40
forcedSpawners = 3
playerCount = 2
expectedRandomSpawners = 5

# Creating new map
baseMap = {
    "server ip": "127.0.0.1",
    "server port": "52124",
    "move-zone-width-ratio": 0.03,
    "translation-pixel-per-frame": 30.0,
    "scroll-ratio-per-tick": 0.2,
    "ai-name": "spearAi",
    "world-absolute-width": mapSizeInGameUnits,
    "world-absolute-height": mapSizeInGameUnits,
    "fog-of-war": True
}

asymmetricSpawnerConfig = np.random.random((discreteMapSize, discreteMapSize)) < expectedRandomSpawners / discreteMapSize**2

# Forcing at least a few spawners
asymmetricSpawnerConfig[np.random.randint(0, discreteMapSize, forcedSpawners), np.random.randint(0, discreteMapSize, forcedSpawners)] = True

symmetryFunction = random.choice(
    [
        lambda asymmetric: np.any([asymmetric, asymmetric[::-1]], axis=0),  # x axis
        lambda asymmetric: np.any([asymmetric, asymmetric[:, ::-1]], axis=0),  # y axis
        lambda asymmetric: np.any([asymmetric, asymmetric.T], axis=0),  # 1, 1 diagonal
        lambda asymmetric: np.any([asymmetric, asymmetric[::-1].T[::-1]], axis=0),  # 1, -1 diagonal
    ]
)

symmetricSpawnerConfig = symmetryFunction(asymmetricSpawnerConfig)

# Generating home spawners
possibleHomes = []

for squareIndex in range(0, np.prod(symmetricSpawnerConfig.shape)):
    if np.count_nonzero(symmetryFunction(arrays.assign(np.zeros((discreteMapSize, discreteMapSize), dtype=bool).reshape(-1), squareIndex, True).reshape(discreteMapSize, -1))) == playerCount:
        possibleHomes.append(squareIndex)

homeSquareIndex = np.random.choice(possibleHomes)
homeSpawnerConfig = symmetryFunction(arrays.assign(np.zeros((discreteMapSize, discreteMapSize), dtype=bool).reshape(-1), homeSquareIndex, True).reshape(discreteMapSize, -1))

# Generating final spawner array
gamePositionOffset = mapSizeInGameUnits * (discreteMapSize - 1) / (discreteMapSize * 2)
def discretePositionToGamePosition(pos: int) -> float:
    return 5000 * pos / discreteMapSize + gamePositionOffset

spawnersConfig: List[List[Dict[str, Union[float, int]]]] = [
    [{"x": discretePositionToGamePosition(x), "y": discretePositionToGamePosition(y)} for x, y in zip(*np.nonzero(symmetricSpawnerConfig))]
]

spawnersConfig = spawnersConfig + [[{"x": discretePositionToGamePosition(x), "y": discretePositionToGamePosition(y), "singuities": 100}] for x, y in zip(*np.nonzero(homeSpawnerConfig))]

baseMap["spawners"] = spawnersConfig

# Writing config to latest game
todo

# Copying game from release
todo

# Run game
todo