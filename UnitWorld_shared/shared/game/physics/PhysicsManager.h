#pragma once

#include "shared/game/physics/collisions/CollisionDetectorFactory.h"
#include "shared/game/physics/actualizers/CompleteGameStateActualizer.h"

#include "shared/game/GameManager.h"

#include "commons/CollectionPipe.h"

namespace uw
{
    class PhysicsManager
    {
    public:
        PhysicsManager(std::shared_ptr<GameManager> gameManager, std::shared_ptr<CollisionDetectorFactory> collisionDetectorFactory):
            _gameManager(gameManager),
            _msPerFrame(1000 / PHISICS_FRAME_PER_SECOND),
            _isRunning(true),
            _collisionDetectorsByPlayerId(std::make_shared<std::unordered_map<xg::Guid, std::shared_ptr<CollisionDetector>>>()),
            _neutralCollisionDetector(collisionDetectorFactory->create()),
            _collisionDetectorFactory(collisionDetectorFactory)
        {}

        void startAsync()
        {
            _physicsThread = std::thread([this] { loopPhysics(); });
        }

        void stop()
        {
            _isRunning = false;

            _physicsThread.join();
        }

    private:

        void loopPhysics()
        {
            while (_isRunning) {

                const auto startFrameTime = std::chrono::steady_clock::now();

                processPhysics();

                const auto endFrameTime = std::chrono::steady_clock::now();

                const auto frameTimeInMs = (unsigned int)std::chrono::duration<double, std::milli>(endFrameTime - startFrameTime).count();

                if (frameTimeInMs < _msPerFrame)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(_msPerFrame - frameTimeInMs));
                }
            }
        }

        void processPhysics()
        {
            _gameManager->processCompleteGameStatePhysics([this](std::shared_ptr<CompleteGameState> completeGameState) {
                const unsigned long long frameTimestamp = std::chrono::steady_clock::now().time_since_epoch().count();

                auto& workingPlayers = completeGameState->players();
                auto& workingSpawners = completeGameState->spawners();

                std::unordered_map<xg::Guid, std::vector<CollidablePoint>> collidablePointsByPlayerId(getCollidablePointsByPlayerId(completeGameState));
                std::vector<CollidablePoint> neutralCollidablePoints(getNeutralCollidablePoints(completeGameState));

                std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<Player>>> playersById =
                    &workingPlayers
                    | toUnorderedMap<xg::Guid, std::shared_ptr<Player>>([](auto player) {
                    return player->id();
                });

                std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<Spawner>>> spawnersById =
                    &workingSpawners
                    | toUnorderedMap<xg::Guid, std::shared_ptr<Spawner>>([](auto spawner) { return spawner->id(); });

                auto shootablesById(std::make_shared<std::unordered_map<xg::Guid, std::shared_ptr<UnitWithHealthPoint>>>());
                for (auto player : workingPlayers)
                {
                    auto singuities = player->singuities();
                    for (auto singuity : *singuities)
                    {
                        shootablesById->emplace(std::make_pair<xg::Guid, std::shared_ptr<UnitWithHealthPoint>>(singuity->id(), singuity));
                    }
                }
                for (auto spawner : workingSpawners)
                {
                    shootablesById->emplace(std::make_pair<xg::Guid, std::shared_ptr<UnitWithHealthPoint>>(spawner->id(), spawner));
                }

                for (unsigned int workingPlayerIndex = 0; workingPlayerIndex < workingPlayers.size(); ++workingPlayerIndex)
                {
                    if (_collisionDetectorsByPlayerId->count(workingPlayers[workingPlayerIndex]->id()) == 0)
                    {
                        (*_collisionDetectorsByPlayerId)[workingPlayers[workingPlayerIndex]->id()] = _collisionDetectorFactory->create();
                    }
                }

                auto playerIdBySinguityId = &workingPlayers
                    | flatMap<std::pair<xg::Guid, xg::Guid>>([](auto player) {
                    return player->singuities()
                        | map<std::pair<xg::Guid, xg::Guid>>([player](auto singuity) {
                        return std::make_pair(singuity->id(), player->id());
                    }) | toVector<std::pair<xg::Guid, xg::Guid>>();
                }) | toUnorderedMap<xg::Guid, xg::Guid>([](auto playerIdAndSinguityId) {
                    return playerIdAndSinguityId.first;
                }, [](auto playerIdAndSinguityId) {
                    return playerIdAndSinguityId.second;
                });

                for (auto player : workingPlayers)
                {
                    _collisionDetectorsByPlayerId->at(player->id())->updateAllCollidablePoints(collidablePointsByPlayerId[player->id()]);
                }

                _neutralCollisionDetector->updateAllCollidablePoints(neutralCollidablePoints);

                CompleteGameStateActualizer completeGameStateActualizer(completeGameState);

                completeGameStateActualizer.spawnAll(*playersById, frameTimestamp);
                completeGameStateActualizer.updateShootingAndPhysicsPredictions(_collisionDetectorsByPlayerId, _neutralCollisionDetector, shootablesById, frameTimestamp);
                completeGameStateActualizer.updatePhysics(*spawnersById);
                completeGameStateActualizer.updateSpawnerAllegences();
            });
        }

        static std::unordered_map<xg::Guid, std::vector<CollidablePoint>> getCollidablePointsByPlayerId(std::shared_ptr<CompleteGameState> completeGameState)
        {
            std::unordered_map<xg::Guid, std::vector<CollidablePoint>> collidablePointsByPlayerId;

            for (auto spawner : completeGameState->spawners())
            {
                spawner->allegence().foreach([spawner, &collidablePointsByPlayerId](const SpawnerAllegence& allegence) {
                    collidablePointsByPlayerId[allegence.allegedPlayerId()].emplace_back(spawner->id(), spawner->position());
                });
            }

            for (auto player : completeGameState->players())
            {
                for (auto singuity : *player->singuities())
                {
                    collidablePointsByPlayerId[player->id()].emplace_back(singuity->id(), singuity->position());
                }
            }

            return collidablePointsByPlayerId;
        }

        static std::vector<CollidablePoint> getNeutralCollidablePoints(std::shared_ptr<CompleteGameState> completeGameState)
        {
            std::vector<CollidablePoint> neutralCollidablePoints;

            for (auto spawner : completeGameState->spawners())
            {
                spawner->allegence().orExecute([spawner, &neutralCollidablePoints]() {
                    neutralCollidablePoints.emplace_back(spawner->id(), spawner->position());
                });
            }

            return neutralCollidablePoints;
        }

        static const unsigned int PHISICS_FRAME_PER_SECOND;

        std::thread _physicsThread;
        bool _isRunning;

        const std::shared_ptr<GameManager> _gameManager;
        const unsigned int _msPerFrame;
        const std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<CollisionDetector>>> _collisionDetectorsByPlayerId;
        const std::shared_ptr<CollisionDetector> _neutralCollisionDetector;
        const std::shared_ptr<CollisionDetectorFactory> _collisionDetectorFactory;
    };
}