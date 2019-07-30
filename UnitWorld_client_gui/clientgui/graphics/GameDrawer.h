#pragma once

#include "shared/game/GameManager.h"

#include "clientgui/graphics/canvas/CanvasTransactionGenerator.h"
#include "clientgui/graphics/units/GraphicalSinguity.h"

#include <chrono>
#include <thread>

namespace uw
{
    class GameDrawer
    {
    public:
        GameDrawer(const int& graphicsFramePerSecond, std::shared_ptr<GameManager> gameManager, std::shared_ptr<CanvasTransactionGenerator> canvasTransactionGenerator):
            _isRunning(true),
            _msPerFrame(1000 / graphicsFramePerSecond),
            _gameManager(gameManager),
            _canvasTransactionGenerator(canvasTransactionGenerator)
        {}

        ~GameDrawer()
        {
            stop();
        }

        void stop()
        {
            _isRunning = false;
        }

        void startSync()
        {
            while (_isRunning) {
                const auto startFrameTime = clock();

                loopGraphics();

                const auto endFrameTime = clock();

                const auto frameTimeInMs = (endFrameTime - startFrameTime) / (CLOCKS_PER_SEC / 1000);

                if (frameTimeInMs < _msPerFrame)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(_msPerFrame - frameTimeInMs));
                }
            }
        }

    private:

        void loopGraphics()
        {
            _canvasTransactionGenerator->tryDrawingTransaction([this](std::shared_ptr<SFMLDrawingCanvas> canvas)
            {
                for (auto singuity : _gameManager->singuities())
                {
                    canvas->draw(*(GraphicalSinguity(*singuity).drawable()));
                }
            });
        }

        bool _isRunning;
        const unsigned int _msPerFrame;

        const std::shared_ptr<GameManager> _gameManager;
        const std::shared_ptr<CanvasTransactionGenerator> _canvasTransactionGenerator;
    };
}