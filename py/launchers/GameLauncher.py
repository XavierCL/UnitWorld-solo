import os
import subprocess
import time
from typing import Any, Callable, List

class GameLauncher:
    def __init__(self, lastGamePath: str, pythonExecutablePath: str, javaAiFolder: str):
        self.lastGamePath = lastGamePath
        self.pythonExecutablePath = pythonExecutablePath
        self.javaAiFolder = javaAiFolder

        self.cleanupStack: List[Callable[[], Any]] = []

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
