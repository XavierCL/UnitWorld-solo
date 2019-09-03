#pragma once

#include "shared/game/play/CompleteGameState.h"

#include "shared/game/commands/MoveMobileUnitsToPosition.h"

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
            _completeGameState(std::make_shared<CompleteGameState>(CompleteGameState::empty())),
            _nextCompleteGameState(nullptr),
            _nextCurrentPlayerId(nullptr),
            _nextAddPlayer(nullptr)
        {}

        void setNextPlayer(std::shared_ptr<Player> newPlayer)
        {
            auto oldStoredValue = _nextAddPlayer.exchange(new Player(*newPlayer));
            if (oldStoredValue)
            {
                delete oldStoredValue;
            }
        }

        void setNextPlayers(CompleteGameState&& nextCompleteGameState, const xg::Guid& currentPlayerId)
        {
            auto oldStoredState = _nextCompleteGameState.exchange(new CompleteGameState(std::forward<CompleteGameState>(nextCompleteGameState)));
            if (oldStoredState)
            {
                delete oldStoredState;
            }

            auto oldStoredCurrent = _nextCurrentPlayerId.exchange(new xg::Guid(currentPlayerId));
            if (oldStoredCurrent)
            {
                delete oldStoredCurrent;
            }
        }

        void setNextMobileUnitsDestination(const xg::Guid& playerId, const std::vector<xg::Guid>& mobileUnitIds, const Vector2D& destination)
        {
            immer::set<xg::Guid> mobileUnitSet;
            for (const auto& mobileUnitId : mobileUnitIds)
            {
                mobileUnitSet.insert(mobileUnitId);
            }

            const auto nextCommand(std::make_shared<MoveMobileUnitsToPosition>(playerId, mobileUnitSet, destination));

            std::lock_guard<std::mutex> lockPlayerCommands(_nextCommandsMutex);

            _nextCommands.push_back(nextCommand);
        }

        void addPlayerInputCallback(const xg::Guid& callbackId, const std::function<void(std::shared_ptr<const CompleteGameState>)>& callback)
        {
            std::lock_guard<std::mutex> lockCallbacks(_somePlayerInputCallbackMutex);

            _somePlayerInputCallback[callbackId] = callback;
        }

        void removePlayerInputCallback(const xg::Guid& callbackId)
        {
            std::lock_guard<std::mutex> lockCallbacks(_somePlayerInputCallbackMutex);

            _somePlayerInputCallback.erase(callbackId);
        }

        std::shared_ptr<const CompleteGameState> completeGameState() const
        {
            return _completeGameState;
        }

        Option<std::shared_ptr<Player>> currentPlayer() const
        {
            return _currentPlayerId.map<std::shared_ptr<Player>>([this](const xg::Guid& playerId) {
                auto completeGameState(completeGameState());
                return &completeGameState->players() | first<std::shared_ptr<Player>>([&playerId](std::shared_ptr<Player> player) {
                    return player->id() == playerId;
                });
            });
        }

        void processCompleteGameStatePhysics(const std::function<void(std::shared_ptr<CompleteGameState>)>& processPhysics)
        {
            auto workingGameState(std::make_shared<CompleteGameState>(*_completeGameState));

            auto newPlayer = _nextAddPlayer.exchange(nullptr);
            if (newPlayer)
            {
                workingGameState->addPlayer(std::shared_ptr<Player>(newPlayer));
            }

            const auto truthCompleteGameState = _nextCompleteGameState.exchange(nullptr);
            if (truthCompleteGameState)
            {
                workingGameState.reset(truthCompleteGameState);
            }

            const auto truthNextCurrentPlayerId = _nextCurrentPlayerId.exchange(nullptr);
            if (truthNextCurrentPlayerId)
            {
                _currentPlayerId = Option<xg::Guid>(truthNextCurrentPlayerId);
            }

            std::vector<std::shared_ptr<MoveMobileUnitsToPosition>> localCommands;
            {
                std::lock_guard<std::mutex> lockCommands(_nextCommandsMutex);

                localCommands = _nextCommands;

                _nextCommands.clear();
            }

            for (const auto command : localCommands)
            {
                command->execute(workingGameState);
            }

            processPhysics(workingGameState);

            _completeGameState = workingGameState;

            if (!localCommands.empty() || newPlayer || truthCompleteGameState)
            {
                std::lock_guard<std::mutex> lockCallback(_somePlayerInputCallbackMutex);

                for (const auto callback : _somePlayerInputCallback)
                {
                    callback.second(_completeGameState);
                }
            }
        }

    private:

        static const unsigned int PHISICS_FRAME_PER_SECOND;

        std::atomic<Player*> _nextAddPlayer;
        std::mutex _nextCommandsMutex;
        std::vector<std::shared_ptr<MoveMobileUnitsToPosition>> _nextCommands;
        std::atomic<CompleteGameState*> _nextCompleteGameState;
        std::atomic<xg::Guid*> _nextCurrentPlayerId;

        Option<xg::Guid> _currentPlayerId;
        std::shared_ptr<const CompleteGameState> _completeGameState;

        std::mutex _somePlayerInputCallbackMutex;
        std::unordered_map<xg::Guid, std::function<void(std::shared_ptr<const CompleteGameState>)>> _somePlayerInputCallback;
    };
}