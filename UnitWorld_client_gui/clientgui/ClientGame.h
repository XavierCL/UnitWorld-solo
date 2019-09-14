#pragma once

#include "clientShared/networking/ServerReceiver.h"

#include "WindowManager.h"

#include "shared/game/physics/PhysicsManager.h"
#include "shared/game/GameManager.h"

namespace uw
{
    class ClientGame
    {
    public:

        ClientGame(std::shared_ptr<GameManager> gameManager, std::shared_ptr<PhysicsManager> physicsManager, std::shared_ptr<WindowManager> windowManager, std::shared_ptr<ServerReceiver> serverReceiver) :
            _gameManager(gameManager),
            _physicsManager(physicsManager),
            _windowManager(windowManager),
            _serverReceiver(serverReceiver)
        {}

        void startSync()
        {
            _physicsManager->startAsync();
            _serverReceiver->startAsync();

            _windowManager->startSync();

            _serverReceiver->stop();
            _physicsManager->stop();
        }

    private:

        const std::shared_ptr<GameManager> _gameManager;
        const std::shared_ptr<PhysicsManager> _physicsManager;
        const std::shared_ptr<WindowManager> _windowManager;
        const std::shared_ptr<ServerReceiver> _serverReceiver;
    };
}