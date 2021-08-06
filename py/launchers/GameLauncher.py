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

    def runPythonAiBackground(self):
        aiProcess = subprocess.Popen(f"python {self.pythonExecutablePath}")
        self.cleanupStack.append(lambda: aiProcess.wait())
        self.cleanupStack.append(lambda: aiProcess.kill())

    def runJavaAiBackground(self):
        aiProcess = subprocess.Popen(
            f'"C:\\Program Files (x86)\\Java\\jdk-14.0.2\\bin\\java.exe" "-javaagent:C:\\Program Files\\JetBrains\\IntelliJ IDEA Community Edition 2020.1.4\\lib\\idea_rt.jar=49386:C:\\Program Files\\JetBrains\\IntelliJ IDEA Community Edition 2020.1.4\\bin" -Dfile.encoding=UTF-8 -classpath "D:\\Program Files\\GitHub\\UnitWorld-solo\\jva\\out\\production\\jva;C:\\Users\\John\\.m2\\repository\\com\\jayway\\jsonpath\\json-path\\2.6.0\\json-path-2.6.0.jar;C:\\Users\\John\\.m2\\repository\\net\\minidev\\json-smart\\2.4.7\\json-smart-2.4.7.jar;C:\\Users\\John\\.m2\\repository\\net\\minidev\\accessors-smart\\2.4.7\\accessors-smart-2.4.7.jar;C:\\Users\\John\\.m2\\repository\\org\\ow2\\asm\\asm\\9.1\\asm-9.1.jar;C:\\Users\\John\\.m2\\repository\\org\\slf4j\\slf4j-api\\1.7.30\\slf4j-api-1.7.30.jar" clientAis.MainClientAi',
            cwd=self.javaAiFolder
        )
        self.cleanupStack.append(lambda: aiProcess.wait())
        self.cleanupStack.append(lambda: aiProcess.kill())

    def runCppAiBackground(self):
        aiProcess = subprocess.Popen(os.path.normpath(os.path.join(self.lastGamePath, 'UnitWorld_client_ais.exe')), cwd=os.path.normpath(self.lastGamePath))
        self.cleanupStack.append(lambda: aiProcess.wait())
        self.cleanupStack.append(lambda: aiProcess.kill())

    def runCppClientGuiPlayerBlocking(self):
        subprocess.call(os.path.normpath(os.path.join(self.lastGamePath, 'UnitWorld_client_gui.exe')), cwd=os.path.normpath(self.lastGamePath))

    def runCppClientGuiObserverBlocking(self):
        sleepTimeInSeconds = 4
        print(f"Waiting {sleepTimeInSeconds} seconds for called ais to startup before starting observer...")
        time.sleep(sleepTimeInSeconds)

        subprocess.call(os.path.normpath(os.path.join(self.lastGamePath, 'UnitWorld_client_gui.exe')), cwd=os.path.normpath(self.lastGamePath))

    def __del__(self):
        for cleanupFunction in self.cleanupStack[::-1]:
            cleanupFunction()
