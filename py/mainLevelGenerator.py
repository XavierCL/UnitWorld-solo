import json
import os
import shutil
import random
import subprocess
import time
from typing import Any, Callable, Dict, List, Union

import numpy as np

from utils import arrays

gameExecutablesPath = "../cpp/x64/Release/"
pythonExecutablePath = "mainClientAi.py"
javaAiFolder = "../jva/out/production/jva/clientAis/"
archivedGamesPath = "./games/archived/"
lastGamePath = "./games/latest/"

mapSizeInGameUnits = 5000.0
discreteMapSize = 41
forcedSpawners = 3
playerCount = 2
expectedRandomSpawners = 5

# Clean up last run if it exists
if len([True for file in os.listdir(lastGamePath) if file == "config.json"]) == 1:
    archivedGameIds = [
        int(fileName[4:])
        for fileName
        in os.listdir(archivedGamesPath)
        if len(fileName) >= 4 and fileName[4:].isdigit()
    ]

    archivedGameId = 0 if len(archivedGameIds) == 0 else np.max(archivedGameIds) + 1
    archivedGamePath = os.path.join(archivedGamesPath, f"game{archivedGameId}")
    os.mkdir(archivedGamePath)
    shutil.move(os.path.join(lastGamePath, "config.json"), os.path.join(archivedGamePath, "config.json"))

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
    "fog-of-war": False
}

asymmetricSpawnerConfig = np.random.random((discreteMapSize, discreteMapSize)) < expectedRandomSpawners / discreteMapSize ** 2

# Forcing at least a few spawners
asymmetricSpawnerConfig[np.random.randint(0, discreteMapSize, forcedSpawners), np.random.randint(0, discreteMapSize, forcedSpawners)] = True

symmetryFunction = random.choice(
    [
        lambda asymmetric: np.any([asymmetric, asymmetric[::-1]], axis=0),  # x axis
        lambda asymmetric: np.any([asymmetric, asymmetric[:, ::-1]], axis=0),  # y axis
        lambda asymmetric: np.any([asymmetric, asymmetric.T], axis=0),  # 1, 1 diagonal
        lambda asymmetric: np.any([asymmetric, asymmetric[::-1].T[::-1]], axis=0),  # 1, -1 diagonal
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
homeSpawnerConfig = symmetryFunction(arrays.assign(np.zeros((discreteMapSize, discreteMapSize), dtype=bool).reshape(-1), homeSquareIndex, True).reshape(discreteMapSize, -1))

# Generating final spawner array
viewPointInGameUnit = mapSizeInGameUnits - 500
gamePositionOffset = (mapSizeInGameUnits - viewPointInGameUnit * (discreteMapSize - 1) / discreteMapSize) / 2

def discretePositionToGamePosition(pos: int) -> float:
    return viewPointInGameUnit * pos / discreteMapSize + gamePositionOffset

spawnersConfig: List[List[Dict[str, Union[float, int]]]] = [
    [{"x": discretePositionToGamePosition(x), "y": discretePositionToGamePosition(y)} for x, y in zip(*np.nonzero(symmetricSpawnerConfig))]
]

spawnersConfig = spawnersConfig + [[{"x": discretePositionToGamePosition(x), "y": discretePositionToGamePosition(y), "singuities": 100}] for x, y in
                                   zip(*np.nonzero(homeSpawnerConfig))]

baseMap["spawners"] = spawnersConfig

# Writing config to latest game
with open(os.path.join(lastGamePath, 'config.json'), 'w', encoding='utf-8') as fileHandle:
    json.dump(baseMap, fileHandle, ensure_ascii=False, indent=4)

# Copying game from release
gameFileNames = ["openal32.dll", "sfml-audio-2.dll", "sfml-graphics-2.dll", "sfml-network-2.dll", "sfml-system-2.dll", "sfml-window-2.dll", "UnitWorld_client_ais.exe", "UnitWorld_client_gui.exe", "UnitWorld_server.exe"]
for gameFileName in gameFileNames:
    shutil.copy(os.path.join(gameExecutablesPath, gameFileName), os.path.join(lastGamePath, gameFileName))

# Run game
cleanupStack: List[Callable[[], Any]] = []

def runServerBackground():
    serverProcess = subprocess.Popen(os.path.normpath(os.path.join(lastGamePath, 'UnitWorld_server.exe')), cwd=os.path.normpath(lastGamePath))
    cleanupStack.append(lambda: serverProcess.wait())
    cleanupStack.append(lambda: serverProcess.kill())

    print("Started server. Waiting 3 seconds...")
    time.sleep(3)

def runPythonAiBackground():
    aiProcess = subprocess.Popen(f"python {pythonExecutablePath}")
    cleanupStack.append(lambda: aiProcess.wait())
    cleanupStack.append(lambda: aiProcess.kill())

def runJavaAiBackground():
    aiProcess = subprocess.Popen(f'"C:\\Program Files (x86)\\Java\\jdk-14.0.2\\bin\\java.exe" "-javaagent:C:\\Program Files\\JetBrains\\IntelliJ IDEA Community Edition 2020.1.4\\lib\\idea_rt.jar=49386:C:\\Program Files\\JetBrains\\IntelliJ IDEA Community Edition 2020.1.4\\bin" -Dfile.encoding=UTF-8 -classpath "D:\\Program Files\\GitHub\\UnitWorld-solo\\jva\\out\\production\\jva;C:\\Users\\John\\.m2\\repository\\com\\jayway\\jsonpath\\json-path\\2.6.0\\json-path-2.6.0.jar;C:\\Users\\John\\.m2\\repository\\net\\minidev\\json-smart\\2.4.7\\json-smart-2.4.7.jar;C:\\Users\\John\\.m2\\repository\\net\\minidev\\accessors-smart\\2.4.7\\accessors-smart-2.4.7.jar;C:\\Users\\John\\.m2\\repository\\org\\ow2\\asm\\asm\\9.1\\asm-9.1.jar;C:\\Users\\John\\.m2\\repository\\org\\slf4j\\slf4j-api\\1.7.30\\slf4j-api-1.7.30.jar" clientAis.MainClientAi aiName "clientAis.implementations.ClosestSpawner"', cwd=javaAiFolder)
    cleanupStack.append(lambda: aiProcess.wait())
    cleanupStack.append(lambda: aiProcess.kill())

def runCppAiBackground():
    aiProcess = subprocess.Popen(os.path.normpath(os.path.join(lastGamePath, 'UnitWorld_client_ais.exe')), cwd=os.path.normpath(lastGamePath))
    cleanupStack.append(lambda: aiProcess.wait())
    cleanupStack.append(lambda: aiProcess.kill())

def runCppClientGuiPlayerBlocking():
    subprocess.call(os.path.normpath(os.path.join(lastGamePath, 'UnitWorld_client_gui.exe')), cwd=os.path.normpath(lastGamePath))

def runCppClientGuiObserverBlocking():
    print("Waiting 3 seconds for called ais to startup before starting observer...")
    time.sleep(4)

    subprocess.call(os.path.normpath(os.path.join(lastGamePath, 'UnitWorld_client_gui.exe')), cwd=os.path.normpath(lastGamePath))

runServerBackground()
#runPythonAiBackground()
#runJavaAiBackground()
#runCppAiBackground()
runCppClientGuiObserverBlocking()

for cleanup in cleanupStack[::-1]:
    cleanup()
