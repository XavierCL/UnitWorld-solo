#pragma once

#include "clientgui/networking/ServerReceiver.h"

#include "clientgui/graphics/WindowManager.h"

#include "shared/game/GameManager.h"

namespace uw
{
    class ClientGame
    {
    public:

        ClientGame(std::shared_ptr<GameManager> gameManager, std::shared_ptr<WindowManager> windowManager, std::shared_ptr<ServerReceiver> serverReceiver) :
            _gameManager(gameManager),
            _windowManager(windowManager),
            _serverReceiver(serverReceiver)
        {}

        void startSync()
        {
            _gameManager->startAsync();
            _serverReceiver->startAsync();

            _windowManager->startSync();

            _serverReceiver->stop();
            _gameManager->stop();
        }

    private:

        const std::shared_ptr<GameManager> _gameManager;
        const std::shared_ptr<WindowManager> _windowManager;
        const std::shared_ptr<ServerReceiver> _serverReceiver;
    };
}