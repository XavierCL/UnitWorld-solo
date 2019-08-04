#pragma once

#include "networking/ClientsGameSender.h"
#include "networking/ClientsReceiver.h"

#include "shared/game/GameManager.h"

namespace uw
{
    class ServerGame {
    public:

        ServerGame(std::shared_ptr<GameManager> gameManager, std::shared_ptr<ClientsGameSender> clientGameSender, std::shared_ptr<ClientsReceiver> clientsReceiver) :
            _gameManager(gameManager),
            _clientGameSender(clientGameSender),
            _clientsReceiver(clientsReceiver)
        {}

        void addClient(std::shared_ptr<CommunicationHandler> communicationHandler)
        {
            const auto singuityInitialXPosition = 100.0 * (double)_gameManager->players().size();
            auto newPlayer(std::make_shared<Player>(xg::newGuid(), std::vector<std::shared_ptr<Singuity>> {std::make_shared<Singuity>(Vector2D(singuityInitialXPosition * 100, 0), Vector2D(), Option<Vector2D>(Vector2D(200, 200)))}));
            const PlayerClient playerClient(newPlayer->id(), communicationHandler);

            _gameManager->setNextPlayer(newPlayer);
            _clientGameSender->addClient(playerClient);
            _clientsReceiver->addPlayerClient(playerClient);
        }

        void startAsync()
        {
            _gameManager->startAsync();
            _clientGameSender->startAsync();
        }

        void stop()
        {
            _clientsReceiver->stop();
            _clientGameSender->stop();
            _gameManager->stop();
        }

    private:

        const std::shared_ptr<GameManager> _gameManager;
        const std::shared_ptr<ClientsGameSender> _clientGameSender;
        const std::shared_ptr<ClientsReceiver> _clientsReceiver;
    };
}