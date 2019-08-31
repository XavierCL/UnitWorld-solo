#pragma once

#include "shared/game/physics/CollisionDetectorFactory.h"

#include "shared/game/GameManager.h"

namespace uw
{
    class PhysicsManager
    {
    public:
        PhysicsManager(std::shared_ptr<GameManager> gameManager, std::shared_ptr<CollisionDetectorFactory> collisionDetectorFactory):
            _msPerFrame(1000 / PHISICS_FRAME_PER_SECOND),
            _isRunning(true),
            _collisionDetectorsByPlayerId(std::make_shared<std::unordered_map<xg::Guid, std::shared_ptr<CollisionDetector>>>()),
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

                for (unsigned int workingPlayerIndex = 0; workingPlayerIndex < workingPlayers->size(); ++workingPlayerIndex)
                {
                    (*workingPlayers)[workingPlayerIndex] = std::make_shared<Player>(*(*workingPlayers)[workingPlayerIndex]);
                    if (_collisionDetectorsByPlayerId->count((*workingPlayers)[workingPlayerIndex]->id()) == 0)
                    {
                        (*_collisionDetectorsByPlayerId)[(*workingPlayers)[workingPlayerIndex]->id()] = _collisionDetectorFactory->create();
                    }
                }

                _playerIdBySinguityId = workingPlayers
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

                for (const auto& player : *workingPlayers)
                {
                    const auto collidablePoints(player->singuities() | map<CollidablePoint>([](auto singuity) {
                        return CollidablePoint(singuity->id(), singuity->position());
                    }) | toVector<CollidablePoint>());
                    _collisionDetectorsByPlayerId->at(player->id())->updateAllCollidablePoints(collidablePoints);
                }

                auto allSinguitiesById = workingPlayers | flatMap<std::shared_ptr<Singuity>>([](auto player) {
                    return player->singuities();
                }) | toUnorderedMap<xg::Guid, std::shared_ptr<Singuity>>([](auto singuity) { return singuity->id(); });

                {
                    auto allSinguities = allSinguitiesById | mapValues<std::shared_ptr<Singuity>>();
                    for (const auto& processingSinguity : *allSinguities)
                    {
                        Option<std::shared_ptr<Singuity>> closestSinguity;
                        Option<std::shared_ptr<Singuity>> closestEnemySinguity;
                        for (const auto& playerIdAndCollisionDetector : *_collisionDetectorsByPlayerId)
                        {
                            auto& collisionDetectorPlayerId(playerIdAndCollisionDetector.first);
                            auto& collisionDetector(playerIdAndCollisionDetector.second);
                            Option<xg::Guid> localClosestSinguityId = collisionDetector->getClosest(CollidablePoint(processingSinguity->id(), processingSinguity->position()));

                            auto localClosestSinguity = localClosestSinguityId.flatMap<std::shared_ptr<Singuity>>([&allSinguitiesById](const auto& singuityId) {
                                return allSinguitiesById | find<std::shared_ptr<Singuity>>(singuityId);
                            });

                            localClosestSinguity.foreach([this, &closestSinguity, &closestEnemySinguity, &processingSinguity, &collisionDetectorPlayerId](const auto& foundSinguity) {
                                closestSinguity = closestSinguity.map<std::shared_ptr<Singuity>>([foundSinguity, processingSinguity](auto singuity) {
                                    return processingSinguity->position().distanceSq(singuity->position()) > processingSinguity->position().distanceSq(foundSinguity->position())
                                        ? foundSinguity
                                        : singuity;
                                }).orElse(Options::Some(foundSinguity));

                                if (collisionDetectorPlayerId != _playerIdBySinguityId->at(processingSinguity->id()))
                                {
                                    closestEnemySinguity = closestEnemySinguity.map<std::shared_ptr<Singuity>>([foundSinguity, processingSinguity](auto singuity) {
                                        return processingSinguity->position().distanceSq(singuity->position()) > processingSinguity->position().distanceSq(foundSinguity->position())
                                            ? foundSinguity
                                            : singuity;
                                    }).orElse(Options::Some(foundSinguity));
                                }
                            });
                        }

                        if (processingSinguity->canShoot())
                        {
                            closestEnemySinguity.foreach([&processingSinguity, &frameTimestamp](auto singuity) {
                                if (processingSinguity->position().distanceSq(singuity->position()) < 700)
                                {
                                    processingSinguity->shoot(singuity, frameTimestamp);
                                }
                            });
                        }

                        closestSinguity.foreach([&processingSinguity](auto singuity) {
                            if (processingSinguity->position().distanceSq(singuity->position()) < 1000)
                            {
                                processingSinguity->setExternalForce((processingSinguity->position() - singuity->position()).divide(0.005, 10.0));
                            }
                        });
                    }
                }

                for (auto workingPlayer : *workingPlayers)
                {
                    workingPlayer->actualize();
                }
            });
        }

        static const unsigned int PHISICS_FRAME_PER_SECOND;

        std::thread _physicsThread;
        bool _isRunning;

        const std::shared_ptr<GameManager> _gameManager;
        const unsigned int _msPerFrame;
        const std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<CollisionDetector>>> _collisionDetectorsByPlayerId;
        const std::shared_ptr<CollisionDetectorFactory> _collisionDetectorFactory;
    };
}