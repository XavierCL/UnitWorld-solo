#pragma once

#include "play/Player.h"
#include "physics/CollisionDetectorFactory.h"
#include "physics/CollisionDetector.h"
#include "physics/CollidablePoint.h"

#include "shared/game/commands/MoveMobileUnitsToPosition.h"

#include "commons/CollectionPipe.h"
#include "commons/MemoryExtension.h"

#include <immer/vector.hpp>

#include <memory>
#include <atomic>
#include <mutex>

namespace uw
{
    class GameManager
    {
    public:
        GameManager(std::shared_ptr<CollisionDetectorFactory> collisionDetectorFactory) :
            _msPerFrame(1000 / PHISICS_FRAME_PER_SECOND),
            _isRunning(true),
            _nextPlayers(nullptr),
            _nextAddedPlayer(nullptr),
            _collisionDetectorsByPlayerId(std::make_shared<std::unordered_map<xg::Guid, std::shared_ptr<CollisionDetector>>>()),
            _collisionDetectorFactory(collisionDetectorFactory)
        {}

        void stop()
        {
            _isRunning = false;

            _physicsThread.join();
        }

        void startAsync()
        {
            _physicsThread = std::thread([this] { loopPhysics(); });
        }

        void setNextAddedPlayer(std::shared_ptr<Player> newPlayer)
        {
            _nextAddedPlayer.store(new Player(*newPlayer));
        }

        void setNextPlayers(immer::vector<std::shared_ptr<Player>> nextPlayers)
        {
            _nextPlayers.store(new immer::vector<std::shared_ptr<Player>>(nextPlayers.begin(), nextPlayers.end()));
        }

        void setNextMobileUnitsDestination(xg::Guid playerId, std::vector<xg::Guid> mobileUnitIds, const Vector2D& destination)
        {
            const auto nextCommand(std::make_shared<MoveMobileUnitsToPosition>(playerId, mobileUnitIds, destination));

            std::lock_guard<std::mutex> lockPlayerCommands(_nextCommandsMutex);

            _nextCommands.push_back(nextCommand);
        }

        void addPlayerInputCallback(const xg::Guid& callbackId, const std::function<void(const immer::vector<std::shared_ptr<Player>>&)>& callback)
        {
            std::lock_guard<std::mutex> lockCallbacks(_somePlayerInputCallbackMutex);

            _somePlayerInputCallback[callbackId] = callback;
        }

        void removePlayerInputCallback(const xg::Guid& callbackId)
        {
            std::lock_guard<std::mutex> lockCallbacks(_somePlayerInputCallbackMutex);

            _somePlayerInputCallback.erase(callbackId);
        }

        immer::vector<std::shared_ptr<Player>> players()
        {
            return _players;
        }

        immer::vector<std::shared_ptr<const Singuity>> singuities()
        {
            const auto localPlayers = _players;

            std::vector<std::shared_ptr<const Singuity>> singuities;
            for (auto player : localPlayers)
            {
                auto playerSinguities = player->singuities();
                singuities.insert(singuities.end(), playerSinguities->begin(), playerSinguities->end());
            }

            return immer::vector<std::shared_ptr<const Singuity>>(singuities.begin(), singuities.end());
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
            const unsigned long long frameTimestamp = std::chrono::steady_clock::now().time_since_epoch().count();

            const auto newPlayer = _nextAddedPlayer.exchange(nullptr);
            if (newPlayer)
            {
                _players = _players.push_back(std::shared_ptr<Player>(newPlayer));
            }

            const auto allPlayers = _nextPlayers.exchange(nullptr);
            if (allPlayers)
            {
                _players = *allPlayers;
                delete allPlayers;
            }

            auto localPlayers = _players;
            auto workingPlayers(std::make_shared<std::vector<std::shared_ptr<Player>>>(localPlayers.begin(), localPlayers.end()));

            for (unsigned int workingPlayerIndex = 0; workingPlayerIndex < workingPlayers->size(); ++workingPlayerIndex)
            {
                (*workingPlayers)[workingPlayerIndex] = std::make_shared<Player>(*(*workingPlayers)[workingPlayerIndex]);
                if (_collisionDetectorsByPlayerId->count((*workingPlayers)[workingPlayerIndex]->id()) == 0)
                {
                    (*_collisionDetectorsByPlayerId)[(*workingPlayers)[workingPlayerIndex]->id()] = _collisionDetectorFactory->create();
                }
            }

            std::vector<std::shared_ptr<MoveMobileUnitsToPosition>> localCommands;
            {
                std::lock_guard<std::mutex> lockCommands(_nextCommandsMutex);

                localCommands = _nextCommands;

                _nextCommands.clear();
            }

            for (const auto command : localCommands)
            {
                command->execute((*workingPlayers));
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

            _players = immer::vector<std::shared_ptr<Player>>(workingPlayers->begin(), workingPlayers->end());

            if (!localCommands.empty() || newPlayer || allPlayers)
            {
                std::lock_guard<std::mutex> lockCallback(_somePlayerInputCallbackMutex);

                for (const auto callback : _somePlayerInputCallback)
                {
                    callback.second(_players);
                }
            }
        }

        static const unsigned int PHISICS_FRAME_PER_SECOND;

        const unsigned int _msPerFrame;
        std::thread _physicsThread;
        std::atomic<Player*> _nextAddedPlayer;
        std::mutex _nextCommandsMutex;
        std::vector<std::shared_ptr<MoveMobileUnitsToPosition>> _nextCommands;
        std::atomic<immer::vector<std::shared_ptr<Player>>*> _nextPlayers;

        immer::vector<std::shared_ptr<Player>> _players;
        std::shared_ptr<std::unordered_map<xg::Guid, xg::Guid>> _playerIdBySinguityId;

        std::mutex _somePlayerInputCallbackMutex;
        std::unordered_map<xg::Guid, std::function<void(const immer::vector<std::shared_ptr<Player>>&)>> _somePlayerInputCallback;
        bool _isRunning;

        const std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<CollisionDetector>>> _collisionDetectorsByPlayerId;
        const std::shared_ptr<CollisionDetectorFactory> _collisionDetectorFactory;
    };
}