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
            const auto playerCount = _gameManager->players().size();
            auto newPlayer(std::make_shared<Player>(xg::newGuid(), generatePlayerSinguities(playerCount)));
            const PlayerClient playerClient(newPlayer->id(), communicationHandler);

            _clientGameSender->addClient(playerClient);
            _gameManager->setNextPlayer(newPlayer);
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

        std::vector<std::shared_ptr<Singuity>> generatePlayerSinguities(int playerCount)
        {
            std::vector<std::shared_ptr<Singuity>> singuities;
            for (int x = 0; x < 100; x += 10)
            {
                for (int y = 0; y < 100; y += 10)
                {
                    singuities.emplace_back(std::make_shared<Singuity>(Vector2D(x + 10 + 100 * playerCount, y + 10)));
                }
            }
            return singuities;
        }

        const std::shared_ptr<GameManager> _gameManager;
        const std::shared_ptr<ClientsGameSender> _clientGameSender;
        const std::shared_ptr<ClientsReceiver> _clientsReceiver;
    };
}