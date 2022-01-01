import os
import shutil
import subprocess
import time
from typing import Any, Callable, List

from launchers.MapHandler import MapHandler

class GameLauncher:
    def __init__(self, generateNewMap: bool):
        self.lastGamePath = "./launchers/games/latest/"
        self.pythonExecutablePath = "mainClientAi.py"
        self.javaAiFolder = "../jva"

        self.cleanupStack: List[Callable[[], Any]] = []

        gameExecutablesPath = "../cpp/x64/Release/"

        # Copying game from release
        gameFileNames = ["openal32.dll", "sfml-audio-2.dll", "sfml-graphics-2.dll", "sfml-network-2.dll", "sfml-system-2.dll", "sfml-window-2.dll", "UnitWorld_client_ais.exe",
                         "UnitWorld_client_gui.exe", "UnitWorld_server.exe"]
        for gameFileName in gameFileNames:
            shutil.copy(os.path.join(gameExecutablesPath, gameFileName), os.path.join(self.lastGamePath, gameFileName))

        archivedGamesPath = "./launchers/games/archived/"

        if generateNewMap:
            mapHandler = MapHandler(self.lastGamePath, archivedGamesPath)
            mapHandler.archiveGenerateAndSaveMap()

    def runServerBackground(self):
        serverProcess = subprocess.Popen(os.path.normpath(os.path.join(self.lastGamePath, 'UnitWorld_server.exe')), cwd=os.path.normpath(self.lastGamePath))
        self.cleanupStack.append(lambda: serverProcess.wait())
        self.cleanupStack.append(lambda: serverProcess.kill())

        sleepTimeInSeconds = 3
        print(f"Started server. Waiting {sleepTimeInSeconds} seconds...")
        time.sleep(sleepTimeInSeconds)

    def runPythonAiBackground(self, aiName=None):
        arguments = "" if aiName is None else f"aiName {aiName}"
        aiProcess = subprocess.Popen(f"python {self.pythonExecutablePath} {arguments}")
        self.cleanupStack.append(lambda: aiProcess.wait())
        self.cleanupStack.append(lambda: aiProcess.kill())

    def runJavaAiBackground(self, aiName=None):
        arguments = "" if aiName is None else f"aiName {aiName}"
        aiProcess = subprocess.Popen(
            f'java -jar out\\artifacts\\jva_jar\\jva.jar ' + arguments,
            cwd=self.javaAiFolder
        )
        self.cleanupStack.append(lambda: aiProcess.wait())
        self.cleanupStack.append(lambda: aiProcess.kill())

    def runCppAiBackground(self):
        aiProcess = subprocess.Popen(os.path.normpath(os.path.join(self.lastGamePath, 'UnitWorld_client_ais.exe')), cwd=os.path.normpath(self.lastGamePath))
        self.cleanupStack.append(lambda: aiProcess.wait())
        self.cleanupStack.append(lambda: aiProcess.kill())

    def runCppClientGuiBlocking(self, mode="observer"):
        if mode == "observer":
            sleepTimeInSeconds = 10
            print(f"Waiting {sleepTimeInSeconds} seconds for called ais to startup before starting observer...")
            time.sleep(sleepTimeInSeconds)

        subprocess.call(os.path.normpath(os.path.join(self.lastGamePath, 'UnitWorld_client_gui.exe')), cwd=os.path.normpath(self.lastGamePath))

    def __del__(self):
        for cleanupFunction in self.cleanupStack[::-1]:
            cleanupFunction()
