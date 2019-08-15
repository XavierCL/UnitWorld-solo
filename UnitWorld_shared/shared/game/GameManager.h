#pragma once

#include "play/Player.h"
#include "physics/CollidablePoint.h"
#include "physics/CollisionDetector.h"

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
        GameManager():
            _msPerFrame(1000 / PHISICS_FRAME_PER_SECOND),
            _isRunning(true),
            _nextPlayers(nullptr),
            _nextAddPlayer(nullptr)
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

        void setNextPlayer(std::shared_ptr<Player> newPlayer)
        {
            _nextAddPlayer.store(new Player(*newPlayer));
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
                singuities.insert(singuities.end(), playerSinguities.begin(), playerSinguities.end());
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
            const auto newPlayer = _nextAddPlayer.exchange(nullptr);
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

            for(unsigned int workingPlayerIndex = 0; workingPlayerIndex < workingPlayers->size(); ++workingPlayerIndex)
            {
                (*workingPlayers)[workingPlayerIndex] = std::make_shared<Player>(*(*workingPlayers)[workingPlayerIndex]);
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

            // todo update player ids by singuity id

            for (const auto& player : *workingPlayers)
            {
                const auto collidablePoints(make_shared(player->singuities()) | map<CollidablePoint>([](const auto& singuity) {
                    return CollidablePoint(singuity->id(), singuity->position());
                }) | toVector<CollidablePoint>());
                _collisionDetectorsByPlayerId->at(player->id())->updateAllCollidablePoints(collidablePoints);
            }

            auto allSinguitiesById = workingPlayers | flatMap<std::shared_ptr<Singuity>>([](const auto& player) {
                return make_shared(player->singuities());
            }) | toUnorderedMap<xg::Guid, std::shared_ptr<Singuity>>([](const auto& singuity) { return singuity->id(); });

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
                        xg::Guid singuityId = collisionDetector->getClosest(processingSinguity->position());
                        auto localClosestSinguity = allSinguitiesById | find<std::shared_ptr<Singuity>>(singuityId);
                        localClosestSinguity.foreach([this, &closestSinguity, &closestEnemySinguity, &processingSinguity, &collisionDetectorPlayerId](const auto& foundSinguity) {
                            closestSinguity = closestSinguity.map<std::shared_ptr<Singuity>>([&foundSinguity, &processingSinguity](const auto& singuity) {
                                return processingSinguity->position().distanceSq(singuity->position()) > processingSinguity->position().distanceSq(foundSinguity->position())
                                    ? foundSinguity
                                    : singuity;
                            }).orElse(Options::Some(foundSinguity));

                            if (collisionDetectorPlayerId != _playerIdBySinguityId->at(processingSinguity->id()))
                            {
                                closestEnemySinguity = closestEnemySinguity.map<std::shared_ptr<Singuity>>([&foundSinguity, &processingSinguity](const auto& singuity) {
                                    return processingSinguity->position().distanceSq(singuity->position()) > processingSinguity->position().distanceSq(foundSinguity->position())
                                        ? foundSinguity
                                        : singuity;
                                }).orElse(Options::Some(foundSinguity));
                            }
                        });
                    }

                    // If closest enemy close enough and processing singuity can shoot, mark for shooting

                    // If closest singuity close enough, accelerate outward
                }
            }

            for (auto workingPlayer : *workingPlayers)
            {
                // reduce hp based on shooting, kill killed singuities, process
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
        std::atomic<Player*> _nextAddPlayer;
        std::mutex _nextCommandsMutex;
        std::vector<std::shared_ptr<MoveMobileUnitsToPosition>> _nextCommands;
        std::atomic<immer::vector<std::shared_ptr<Player>>*> _nextPlayers;

        immer::vector<std::shared_ptr<Player>> _players;
        std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<CollisionDetector>>> _collisionDetectorsByPlayerId;
        std::shared_ptr<std::unordered_map<xg::Guid, xg::Guid>> _playerIdBySinguityId;

        std::mutex _somePlayerInputCallbackMutex;
        std::unordered_map<xg::Guid, std::function<void(const immer::vector<std::shared_ptr<Player>>&)>> _somePlayerInputCallback;
        bool _isRunning;
    };
}