#pragma once

#include "shared/game/play/CompleteGameState.h"

#include "shared/game/commands/MoveMobileUnitsToPosition.h"
#include "shared/game/commands/MoveMobileUnitsToSpawner.h"
#include "shared/game/commands/SetSpawnersRally.h"
#include "shared/game/commands/GameCommand.h"

#include <immer/vector.hpp>
#include <immer/set.hpp>

#include "commons/CollectionPipe.h"

#include <memory>
#include <atomic>
#include <mutex>

namespace uw
{
    class GameManager
    {
    public:
        GameManager() :
            _localCompleteGameState(std::make_shared<CompleteGameState>(CompleteGameState::empty())),
            _completeGameState(std::make_shared<CompleteGameState>(CompleteGameState::empty())),
            _independentCompleteGameState(std::make_shared<CompleteGameState>(CompleteGameState::empty())),
            _nextCompleteGameState(nullptr),
            _nextCurrentPlayerId(nullptr),
            _nextAddPlayer(nullptr),
            _nextAddSpawners(nullptr),
            _independentGameWasAltered(false),
            _nextCommandsMutex(std::make_shared<std::unordered_map<xg::Guid, std::shared_ptr<std::mutex>>>()),
            _nextCommands(std::make_shared<std::unordered_map<xg::Guid, std::shared_ptr<std::vector<std::shared_ptr<GameCommand>>>>>())
        {}

        void setNextPlayer(std::shared_ptr<Player> newPlayer, const std::vector<std::shared_ptr<Spawner>>& spawners)
        {
            auto oldStoredPlayer = _nextAddPlayer.exchange(new Player(*newPlayer));
            if (oldStoredPlayer)
            {
                delete oldStoredPlayer;
            }

            auto oldStoredSpawners = _nextAddSpawners.exchange(new std::vector<std::shared_ptr<Spawner>>(spawners));
            if (oldStoredSpawners)
            {
                delete oldStoredSpawners;
            }
        }

        void setNextCompleteGameState(CompleteGameState&& nextCompleteGameState)
        {
            auto oldStoredState = _nextCompleteGameState.exchange(new CompleteGameState(std::forward<CompleteGameState>(nextCompleteGameState)));
            if (oldStoredState)
            {
                delete oldStoredState;
            }
        }

        void setNextCurrentPlayerId(const xg::Guid& currentPlayerId)
        {
            auto oldStoredCurrent = _nextCurrentPlayerId.exchange(new xg::Guid(currentPlayerId));
            if (oldStoredCurrent)
            {
                delete oldStoredCurrent;
            }
        }

        void setNextMobileUnitsDestination(const xg::Guid& playerId, const std::vector<xg::Guid>& mobileUnitIds, const Vector2D& destination)
        {
            addPlayerInCommandQueueIfNotThere(playerId);

            immer::set<xg::Guid> mobileUnitSet;
            for (const auto& mobileUnitId : mobileUnitIds)
            {
                mobileUnitSet = std::move(mobileUnitSet).insert(mobileUnitId);
            }

            const auto nextCommand(std::make_shared<MoveMobileUnitsToPosition>(playerId, mobileUnitSet, destination));

            const auto localNextCommandsMutex = _nextCommandsMutex;
            std::lock_guard<std::mutex> lockPlayerCommands(*localNextCommandsMutex->at(playerId));

            const auto localNextCommands = _nextCommands;
            localNextCommands->at(playerId)->push_back(nextCommand);
        }

        void setNextMobileUnitsSpawnerDestination(const xg::Guid& playerId, const std::vector<xg::Guid>& mobileUnitIds, const xg::Guid& spawnerId)
        {
            addPlayerInCommandQueueIfNotThere(playerId);

            immer::set<xg::Guid> mobileUnitSet;
            for (const auto& mobileUnitId : mobileUnitIds)
            {
                mobileUnitSet = std::move(mobileUnitSet).insert(mobileUnitId);
            }

            const auto nextCommand(std::make_shared<MoveMobileUnitsToSpawner>(playerId, mobileUnitSet, spawnerId));

            const auto localNextCommandsMutex = _nextCommandsMutex;
            std::lock_guard<std::mutex> lockPlayerCommands(*localNextCommandsMutex->at(playerId));

            const auto localNextCommands = _nextCommands;
            localNextCommands->at(playerId)->push_back(nextCommand);
        }

        void setNextSpawnersRally(const xg::Guid& playerId, const std::vector<xg::Guid>& spawnersId, const MobileUnitDestination& rally)
        {
            addPlayerInCommandQueueIfNotThere(playerId);

            immer::set<xg::Guid> spawnersIdSet;
            for (const auto& spawnerId : spawnersId)
            {
                spawnersIdSet = std::move(spawnersIdSet).insert(spawnerId);
            }

            const auto nextCommand(std::make_shared<SetSpawnersRally>(playerId, spawnersIdSet, rally));

            const auto localNextCommandsMutex = _nextCommandsMutex;
            std::lock_guard<std::mutex> lockPlayerCommands(*localNextCommandsMutex->at(playerId));

            const auto localNextCommands = _nextCommands;
            localNextCommands->at(playerId)->push_back(nextCommand);
        }

        void addPlayerInputCallback(const xg::Guid& callbackId, const std::function<void()>& callback)
        {
            std::lock_guard<std::mutex> lockCallbacks(_somePlayerInputCallbackMutex);

            _somePlayerInputCallback[callbackId] = callback;
        }

        void removePlayerInputCallback(const xg::Guid& callbackId)
        {
            std::lock_guard<std::mutex> lockCallbacks(_somePlayerInputCallbackMutex);

            _somePlayerInputCallback.erase(callbackId);
        }

        void addNewIndependentGameStateCallback(const xg::Guid& callbackId, const std::function<void()>& callback)
        {
            std::lock_guard<std::mutex> lockCallbacks(_somePlayerInputCallbackMutex);

            _someNewIndependantGameStateCallback[callbackId] = callback;
        }

        void removeNewIndependentGameStateCallback(const xg::Guid& callbackId)
        {
            std::lock_guard<std::mutex> lockCallbacks(_somePlayerInputCallbackMutex);

            _someNewIndependantGameStateCallback.erase(callbackId);
        }

        void addDependentGameStateChangedCallback(const xg::Guid& callbackId, const std::function<void()>& callback)
        {
            std::lock_guard<std::mutex> lockCallbacks(_somePlayerInputCallbackMutex);

            _dependentGameStateChangedCallback[callbackId] = callback;
        }

        void removeDependentGameStateChangedCallback(const xg::Guid& callbackId)
        {
            std::lock_guard<std::mutex> lockCallbacks(_somePlayerInputCallbackMutex);

            _dependentGameStateChangedCallback.erase(callbackId);
        }

        void setNextIndependentCompleteGameState(std::shared_ptr<const CompleteGameState> completeGameState)
        {
            _independentCompleteGameState = completeGameState;

            if (_independentGameWasAltered)
            {
                _independentGameWasAltered = false;

                std::unique_lock<std::mutex> lockCallback(_somePlayerInputCallbackMutex, std::try_to_lock);

                if (lockCallback.owns_lock())
                {
                    for (const auto callback : _somePlayerInputCallback)
                    {
                        callback.second();
                    }
                }
            }
            else
            {
                std::unique_lock<std::mutex> lockCallback(_somePlayerInputCallbackMutex, std::try_to_lock);

                if (lockCallback.owns_lock())
                {
                    for (const auto callback : _someNewIndependantGameStateCallback)
                    {
                        callback.second();
                    }
                }
            }
        }

        std::shared_ptr<const CompleteGameState> independentCompleteGameState() const
        {
            return _independentCompleteGameState;
        }

        std::shared_ptr<const CompleteGameState> completeGameState() const
        {
            return _completeGameState;
        }

        Option<std::shared_ptr<Player>> currentPlayer() const
        {
            return _currentPlayerId.map<std::shared_ptr<Player>>([this](const xg::Guid& playerId) {
                auto completeGameState(_completeGameState);
                return &completeGameState->players() | first<std::shared_ptr<Player>>([&playerId](std::shared_ptr<Player> player) {
                    return player->id() == playerId;
                });
            });
        }

        void processCompleteGameStatePhysics(const std::function<void(std::shared_ptr<CompleteGameState>, const bool)>& processPhysics)
        {
            bool gameAltered = false;

            auto newPlayer = _nextAddPlayer.exchange(nullptr);
            if (newPlayer)
            {
                _localCompleteGameState->addPlayer(std::shared_ptr<Player>(newPlayer));
                gameAltered = true;
            }

            auto newSpawners = _nextAddSpawners.exchange(nullptr);
            if (newSpawners)
            {
                _localCompleteGameState->addSpawners(*newSpawners);
                delete newSpawners;
                gameAltered = true;
            }

            const auto truthCompleteGameState = _nextCompleteGameState.exchange(nullptr);
            if (truthCompleteGameState)
            {
                _localCompleteGameState.reset(truthCompleteGameState);
                _independentCompleteGameState = std::make_shared<const CompleteGameState>(*truthCompleteGameState);
                gameAltered = true;
            }

            const auto truthNextCurrentPlayerId = _nextCurrentPlayerId.exchange(nullptr);
            if (truthNextCurrentPlayerId)
            {
                _currentPlayerId = Option<xg::Guid>(truthNextCurrentPlayerId);
                gameAltered = true;
            }

            std::vector<std::shared_ptr<GameCommand>> localCommands;
            const auto nextCommandsMutexCopy = _nextCommandsMutex;
            for (const auto nextCommandsMutex: *nextCommandsMutexCopy)
            {
                std::unique_lock<std::mutex> lockCommands(*nextCommandsMutex.second, std::try_to_lock);

                if (lockCommands.owns_lock()) {
                    const auto playerNextCommands = _nextCommands->at(nextCommandsMutex.first);
                    localCommands.insert(localCommands.end(), playerNextCommands->begin(), playerNextCommands->end());

                    playerNextCommands->clear();
                }
            }

            for (const auto command : localCommands)
            {
                command->execute(_localCompleteGameState);
            }

            _independentGameWasAltered = _independentGameWasAltered || gameAltered || !localCommands.empty();

            processPhysics(_localCompleteGameState, gameAltered);

            _completeGameState = std::make_shared<const CompleteGameState>(*_localCompleteGameState);

            {
                std::unique_lock<std::mutex> lockCallback(_somePlayerInputCallbackMutex, std::try_to_lock);

                if (lockCallback.owns_lock())
                {
                    for (const auto callback : _dependentGameStateChangedCallback)
                    {
                        callback.second();
                    }
                }
            }
        }

    private:

        void addPlayerInCommandQueueIfNotThere(const xg::Guid& playerId)
        {
            if (_nextCommandsMutex->count(playerId) == 0)
            {
                std::lock_guard<std::mutex> lockPlayerCommands(_nextCommandsModifierMutex);

                auto newNextCommands = std::make_shared<std::unordered_map<xg::Guid, std::shared_ptr<std::vector<std::shared_ptr<GameCommand>>>>>(*_nextCommands);
                newNextCommands->emplace(std::make_pair(playerId, std::make_shared<std::vector<std::shared_ptr<GameCommand>>>()));
                _nextCommands = newNextCommands;

                auto newNextCommandsMutex = std::make_shared<std::unordered_map<xg::Guid, std::shared_ptr<std::mutex>>>(*_nextCommandsMutex);
                newNextCommandsMutex->emplace(std::make_pair(playerId, std::make_shared<std::mutex>()));
                _nextCommandsMutex = newNextCommandsMutex;
            }
        }

        static const unsigned int PHISICS_FRAME_PER_SECOND;

        std::atomic<Player*> _nextAddPlayer;
        std::atomic<std::vector<std::shared_ptr<Spawner>>*> _nextAddSpawners;
        bool _independentGameWasAltered;
        std::mutex _nextCommandsModifierMutex;
        std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<std::mutex>>> _nextCommandsMutex;
        std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<std::vector<std::shared_ptr<GameCommand>>>>> _nextCommands;
        std::atomic<CompleteGameState*> _nextCompleteGameState;
        std::atomic<xg::Guid*> _nextCurrentPlayerId;

        Option<xg::Guid> _currentPlayerId;
        std::shared_ptr<CompleteGameState> _localCompleteGameState;
        std::shared_ptr<const CompleteGameState> _completeGameState;
        std::shared_ptr<const CompleteGameState> _independentCompleteGameState;

        std::mutex _somePlayerInputCallbackMutex;
        std::unordered_map<xg::Guid, std::function<void()>> _somePlayerInputCallback;
        std::unordered_map<xg::Guid, std::function<void()>> _someNewIndependantGameStateCallback;
        std::unordered_map<xg::Guid, std::function<void()>> _dependentGameStateChangedCallback;
    };
}