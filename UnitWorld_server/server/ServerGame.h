#pragma once

#include "networking/ClientsGameSender.h"
#include "networking/ClientsReceiver.h"

#include "shared/game/physics/PhysicsManager.h"

#include "shared/game/GameManager.h"

namespace uw
{
    class ServerGame {
    public:

        ServerGame(std::shared_ptr<GameManager> gameManager, std::shared_ptr<PhysicsManager> physicsManager, std::shared_ptr<ClientsGameSender> clientGameSender, std::shared_ptr<ClientsReceiver> clientsReceiver, const std::vector<Vector2D>& firstSpawners) :
            _gameManager(gameManager),
            _physicsManager(physicsManager),
            _clientGameSender(clientGameSender),
            _clientsReceiver(clientsReceiver),
            _firstSpawners(firstSpawners)
        {}

        void addClient(std::shared_ptr<CommunicationHandler> communicationHandler)
        {
            const auto playerCount = _gameManager->completeGameState()->players().size();

            auto newPlayer(std::make_shared<Player>(xg::newGuid(), generatePlayerSinguities(playerCount)));
            auto newSpawners(generatePlayerSpawners(newPlayer->id(), playerCount));
            const PlayerClient playerClient(newPlayer->id(), communicationHandler);

            _clientGameSender->addClient(playerClient);
            _gameManager->setNextPlayer(newPlayer, newSpawners);
            _clientsReceiver->addPlayerClient(playerClient);
        }

        void startAsync()
        {
            _physicsManager->startAsync();
            _clientGameSender->startAsync();
        }

        void stop()
        {
            _clientsReceiver->stop();
            _clientGameSender->stop();
            _physicsManager->stop();
        }

    private:

        std::vector<std::shared_ptr<Singuity>> generatePlayerSinguities(int playerCount)
        {
            if (playerCount >= _firstSpawners.size())
            {
                return std::vector<std::shared_ptr<Singuity>> {};
            }
            else
            {
                Vector2D averageSinguityPosition = _firstSpawners[playerCount];

                std::vector<std::shared_ptr<Singuity>> singuities;
                for (int x = averageSinguityPosition.x() - 50; x < averageSinguityPosition.x() + 50; x += 10)
                {
                    for (int y = averageSinguityPosition.y() - 50; y < averageSinguityPosition.y() + 50; y += 10)
                    {
                        singuities.emplace_back(std::make_shared<Singuity>(Vector2D(x, y)));
                    }
                }
                return singuities;
            }
        }

        std::vector<std::shared_ptr<Spawner>> generatePlayerSpawners(const xg::Guid& playerId, int playerCount)
        {
            if (playerCount >= _firstSpawners.size())
            {
                return std::vector<std::shared_ptr<Spawner>> {};
            }
            else
            {
                return std::vector<std::shared_ptr<Spawner>> { std::make_shared<Spawner>(_firstSpawners[playerCount], playerId) };
            }
        }

        const std::shared_ptr<GameManager> _gameManager;
        const std::shared_ptr<PhysicsManager> _physicsManager;
        const std::shared_ptr<ClientsGameSender> _clientGameSender;
        const std::shared_ptr<ClientsReceiver> _clientsReceiver;
        const std::vector<Vector2D> _firstSpawners;
    };
}