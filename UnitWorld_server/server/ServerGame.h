#pragma once

#include "networking/ClientsGameSender.h"
#include "networking/ClientsReceiver.h"

#include "shared/game/physics/PhysicsManager.h"

#include "shared/game/GameManager.h"

namespace uw
{
    class ServerGame {
    public:

        ServerGame(std::shared_ptr<GameManager> gameManager, std::shared_ptr<PhysicsManager> physicsManager, std::shared_ptr<ClientsGameSender> clientGameSender, std::shared_ptr<ClientsReceiver> clientsReceiver, const std::vector<std::vector<Vector2D>>& firstSpawners, const std::vector<std::vector<size_t>>& singuitiesBySpawner) :
            _gameManager(gameManager),
            _physicsManager(physicsManager),
            _clientGameSender(clientGameSender),
            _clientsReceiver(clientsReceiver),
            _firstSpawners(firstSpawners),
            _singuitiesBySpawner(singuitiesBySpawner)
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
            _gameManager->setNextCompleteGameState(generateInitialMap());

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
            if (playerCount >= _singuitiesBySpawner.size())
            {
                return std::vector<std::shared_ptr<Singuity>> {};
            }
            else
            {
                std::vector<std::shared_ptr<Singuity>> singuities;

                for (size_t spawnerCount = 0; spawnerCount < _firstSpawners[playerCount + 1].size(); ++spawnerCount)
                {
                    Vector2D averageSinguityPosition = _firstSpawners[playerCount + 1][spawnerCount];
                    size_t singuityCount = _singuitiesBySpawner[playerCount][spawnerCount];
                    if (singuityCount > 0)
                    {
                        for (int x = averageSinguityPosition.x() - 50; x < averageSinguityPosition.x() + 50; x += 100 / sqrt(singuityCount))
                        {
                            for (int y = averageSinguityPosition.y() - 50; y < averageSinguityPosition.y() + 50; y += 100 / sqrt(singuityCount))
                            {
                                singuities.emplace_back(std::make_shared<Singuity>(Vector2D(x, y)));
                            }
                        }
                    }
                }

                return singuities;
            }
        }

        std::vector<std::shared_ptr<Spawner>> generatePlayerSpawners(const xg::Guid& playerId, int playerCount)
        {
            auto spawnerIndex = playerCount + 1;
            if (spawnerIndex >= _firstSpawners.size())
            {
                return std::vector<std::shared_ptr<Spawner>> {};
            }
            else
            {
                std::vector<std::shared_ptr<Spawner>> spawners;
                for (const auto& initialSpawnerLocation : _firstSpawners[spawnerIndex])
                {
                    spawners.emplace_back(std::make_shared<Spawner>(initialSpawnerLocation, playerId));
                }
                return spawners;
            }
        }

        CompleteGameState generateInitialMap() const
        {
            std::vector<std::shared_ptr<Spawner>> spawners;

            if (_firstSpawners.size() > 0)
            {
                for (const auto& emptySpawnerCoordinate : _firstSpawners.front())
                {
                    spawners.emplace_back(std::make_shared<Spawner>(emptySpawnerCoordinate));
                }
            }

            return CompleteGameState(std::move(spawners), std::vector<std::shared_ptr<Player>> {}, 0);
        }

        const std::shared_ptr<GameManager> _gameManager;
        const std::shared_ptr<PhysicsManager> _physicsManager;
        const std::shared_ptr<ClientsGameSender> _clientGameSender;
        const std::shared_ptr<ClientsReceiver> _clientsReceiver;
        const std::vector<std::vector<Vector2D>> _firstSpawners;
        const std::vector<std::vector<size_t>> _singuitiesBySpawner;
    };
}