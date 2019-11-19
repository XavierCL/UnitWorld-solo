#pragma once

#include "PhysicsStats.h"

#include "shared/game/physics/collisions/CollisionDetectorFactory.h"
#include "shared/game/physics/actualizers/CompleteGameStateActualizer.h"

#include "shared/game/GameManager.h"

#include "commons/CollectionPipe.h"

namespace uw
{
    class PhysicsManager
    {
    public:
        PhysicsManager(std::shared_ptr<GameManager> gameManager, std::shared_ptr<CollisionDetectorFactory> collisionDetectorFactory, std::shared_ptr<PhysicsStats> physicsStats):
            _gameManager(gameManager),
            _msPerFrame(1000 / PHISICS_FRAME_PER_SECOND),
            _collisionsFrameInterval((int)round((double)PHISICS_FRAME_PER_SECOND / (double)COLLISIONS_FRAME_PER_SECOND)),
            _isRunning(true),
            _collisionDetectorsByPlayerId(std::make_shared<std::unordered_map<xg::Guid, std::shared_ptr<CollisionDetector>>>()),
            _neutralCollisionDetector(collisionDetectorFactory->create()),
            _collisionDetectorFactory(collisionDetectorFactory),
            _completeGameStateActualizer(),
            _physicsStats(physicsStats)
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

                _physicsStats->feedFrameDuration(frameTimeInMs);

                if (frameTimeInMs < _msPerFrame)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(_msPerFrame - frameTimeInMs));
                }
            }
        }

        void processPhysics()
        {
            _gameManager->processCompleteGameStatePhysics([this](std::shared_ptr<CompleteGameState> completeGameState, const bool gameRefreshed) {

                auto& workingPlayers = completeGameState->players();
                auto& workingSpawners = completeGameState->spawners();

                std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<Player>>> playersById =
                    &workingPlayers
                    | toUnorderedMap<xg::Guid, std::shared_ptr<Player>>([](auto player) {
                    return player->id();
                });

                if (gameRefreshed || !_completeGameStateActualizer)
                {
                    _completeGameStateActualizer = std::make_shared<CompleteGameStateActualizer>(completeGameState);
                }

                _completeGameStateActualizer->spawnAll(*playersById, completeGameState->frameCount());

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

                for (auto spawner: workingSpawners)
                {
                    shootablesById->emplace(std::make_pair<xg::Guid, std::shared_ptr<UnitWithHealthPoint>>(spawner->id(), spawner));
                }

                std::unordered_map<xg::Guid, xg::Guid> playerIdBySinguityId;
                playerIdBySinguityId.reserve(shootablesById->size());
                for (const auto& player : workingPlayers)
                {
                    for (const auto& singuity : *player->singuities())
                    {
                        playerIdBySinguityId.emplace(std::make_pair(singuity->id(), player->id()));
                    }
                }

                if (completeGameState->frameCount() % _collisionsFrameInterval == 0)
                {
                    // Computing the collisions

                    std::unordered_map<xg::Guid, std::vector<CollidablePoint>> collidablePointsByPlayerId(getCollidablePointsByPlayerId(completeGameState));

                    for (unsigned int workingPlayerIndex = 0; workingPlayerIndex < workingPlayers.size(); ++workingPlayerIndex)
                    {
                        if (_collisionDetectorsByPlayerId->count(workingPlayers[workingPlayerIndex]->id()) == 0)
                        {
                            (*_collisionDetectorsByPlayerId)[workingPlayers[workingPlayerIndex]->id()] = _collisionDetectorFactory->create();
                        }
                    }

                    for (auto spawner : workingSpawners)
                    {
                        spawner->allegence().foreach([&collidablePointsByPlayerId, spawner](const SpawnerAllegence& allegence) {
                            collidablePointsByPlayerId[allegence.allegedPlayerId()].emplace_back(spawner->id(), spawner->position());
                        });
                    }

                    for (auto player : workingPlayers)
                    {
                        _collisionDetectorsByPlayerId->at(player->id())->updateAllCollidablePoints(collidablePointsByPlayerId[player->id()]);
                    }

                    std::vector<CollidablePoint> collidablePoints;
                    for (auto spawner : completeGameState->spawners())
                    {
                        collidablePoints.emplace_back(spawner->id(), spawner->position());
                    }

                    const auto spawnerCollisionDetector = _collisionDetectorFactory->create();
                    spawnerCollisionDetector->updateAllCollidablePoints(collidablePoints);

                    _completeGameStateActualizer->updateCollisions(_collisionDetectorsByPlayerId, shootablesById, spawnerCollisionDetector, *spawnersById);
                }

                _completeGameStateActualizer->shootEnemies(shootablesById);
                _completeGameStateActualizer->updatePhysics(*spawnersById, shootablesById);
                _completeGameStateActualizer->updateSpawnerAllegences();

                completeGameState->incrementFrameCount();

                if (completeGameState->frameCount() % _collisionsFrameInterval == 0)
                {
                    _gameManager->setNextIndependentCompleteGameState(std::make_shared<const CompleteGameState>(*completeGameState));
                }
            });
        }

        static std::unordered_map<xg::Guid, std::vector<CollidablePoint>> getCollidablePointsByPlayerId(std::shared_ptr<CompleteGameState> completeGameState)
        {
            std::unordered_map<xg::Guid, std::vector<CollidablePoint>> collidablePointsByPlayerId;

            for (auto player : completeGameState->players())
            {
                for (auto singuity : *player->singuities())
                {
                    collidablePointsByPlayerId[player->id()].emplace_back(singuity->id(), singuity->position());
                }
            }

            return collidablePointsByPlayerId;
        }

        static const unsigned int PHISICS_FRAME_PER_SECOND;
        static const unsigned int COLLISIONS_FRAME_PER_SECOND;

        std::thread _physicsThread;
        bool _isRunning;

        const std::shared_ptr<GameManager> _gameManager;
        const unsigned int _msPerFrame;
        const unsigned int _collisionsFrameInterval;
        const std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<CollisionDetector>>> _collisionDetectorsByPlayerId;
        const std::shared_ptr<CollisionDetector> _neutralCollisionDetector;
        const std::shared_ptr<CollisionDetectorFactory> _collisionDetectorFactory;
        const std::shared_ptr<PhysicsStats> _physicsStats;

        std::shared_ptr<CompleteGameStateActualizer> _completeGameStateActualizer;
    };
}