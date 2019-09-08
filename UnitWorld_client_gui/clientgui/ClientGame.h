#pragma once

#include "clientgui/networking/ServerReceiver.h"

#include "clientgui/graphics/WindowManager.h"

#include "shared/game/GameManager.h"

namespace uw
{
    class ClientGame
    {
    public:

        ClientGame(std::shared_ptr<Player> currentPlayer, std::shared_ptr<GameManager> gameManager, std::shared_ptr<WindowManager> windowManager, std::shared_ptr<ServerReceiver> serverReceiver) :
            _currentPlayer(currentPlayer),
            _gameManager(gameManager),
            _windowManager(windowManager),
            _serverReceiver(serverReceiver)
        {}

        void startSync()
        {
            std::thread gameManagerThread([this] { _gameManager->startSync(); });
            std::thread serverReceiverThread([this] { _serverReceiver->startSync(); });

            _windowManager->startSync();

            _serverReceiver->stop();
            serverReceiverThread.join();

            _gameManager->stop();
            gameManagerThread.join();
        }

    private:

        const std::shared_ptr<Player> _currentPlayer;
        const std::shared_ptr<GameManager> _gameManager;
        const std::shared_ptr<WindowManager> _windowManager;
        const std::shared_ptr<ServerReceiver> _serverReceiver;
    };
}