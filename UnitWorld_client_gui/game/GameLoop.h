#pragma once

#include "game/play/Player.h"

#include "../graphics/canvas/CanvasTransactionGenerator.h"

#include "communications/CommunicationHandler.h"

namespace uw
{
    class GameLoop
    {
    public:

        GameLoop(const int& graphicsFramePerSecond, const int& physicsFramePerSecond, std::shared_ptr<CommunicationHandler> serverHandler, std::shared_ptr<CanvasTransactionGenerator> canvasTransactionGenerator);

        void start();

    private:
        void handlePhysics();
        void handleGraphics();

        const std::shared_ptr<CanvasTransactionGenerator> _canvasTransactionGenerator;
        const std::shared_ptr<Player> _currentPlayer;
    };
}