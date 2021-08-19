import os
import shutil

from launchers.GameLauncher import GameLauncher
from launchers.MapHandler import MapHandler

archivedGamesPath = "./launchers/games/archived/"
lastGamePath = "./launchers/games/latest/"
gameExecutablesPath = "../cpp/x64/Release/"

pythonExecutablePath = "mainClientAi.py"
javaAiFolder = "../jva/out/production/jva/clientAis/"

mapHandler = MapHandler(lastGamePath, archivedGamesPath)
mapHandler.archiveLastMap()
mapHandler.generateRandomMap()
mapHandler.saveMapToFile()

# Copying game from release
gameFileNames = ["openal32.dll", "sfml-audio-2.dll", "sfml-graphics-2.dll", "sfml-network-2.dll", "sfml-system-2.dll", "sfml-window-2.dll", "UnitWorld_client_ais.exe",
                 "UnitWorld_client_gui.exe", "UnitWorld_server.exe"]
for gameFileName in gameFileNames:
    shutil.copy(os.path.join(gameExecutablesPath, gameFileName), os.path.join(lastGamePath, gameFileName))

gameLauncher = GameLauncher(lastGamePath, pythonExecutablePath, javaAiFolder)
gameLauncher.runServerBackground()
gameLauncher.runPythonAiBackground(aiName="discreteV1")
gameLauncher.runCppAiBackground()
gameLauncher.runCppClientGuiBlocking(mode="observer")
