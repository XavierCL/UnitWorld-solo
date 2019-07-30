#pragma once

#include "clientgui/networking/ServerReceiver.h"

#include "clientgui/graphics/GameDrawer.h"

#include "clientgui/graphics/canvas/CanvasTransactionGenerator.h"

#include "shared/game/play/Player.h"
#include "shared/game/GameManager.h"

#include "communications/CommunicationHandler.h"

namespace uw
{
    class ClientGame
    {
    public:

        ClientGame(const int& graphicsFramePerSecond, const int& physicsFramePerSecond, std::shared_ptr<Player> currentPlayer, std::shared_ptr<CommunicationHandler> serverHandler, std::shared_ptr<CanvasTransactionGenerator> canvasTransactionGenerator, std::shared_ptr<MessageSerializer> messageSerializer);

        ~ClientGame();

        void startSync();

        void stop();

    private:

        const std::shared_ptr<GameManager> _gameManager;
        const std::shared_ptr<Player> _currentPlayer;
        const std::shared_ptr<CommunicationHandler> _serverHandler;
        GameDrawer _gameDrawer;
        ServerReceiver _serverReceiver;
    };
}