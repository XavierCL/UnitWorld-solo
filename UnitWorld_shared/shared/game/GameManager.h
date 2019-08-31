#pragma once

#include "shared/game/play/CompleteGameState.h"

#include "shared/game/commands/MoveMobileUnitsToPosition.h"

#include <immer/vector.hpp>

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
            _nextAddPlayer(nullptr)
        {}

        void setNextPlayer(std::shared_ptr<Player> newPlayer)
        {
            _nextAddPlayer.store(new Player(*newPlayer));
        }

        void setNextPlayers(CompleteGameState&& nextCompleteGameState)
        {
            _nextCompleteGameState.store(new CompleteGameState(std::forward<CompleteGameState>(nextCompleteGameState)));
        }

        void setNextMobileUnitsDestination(xg::Guid playerId, std::vector<xg::Guid> mobileUnitIds, const Vector2D& destination)
        {
            const auto nextCommand(std::make_shared<MoveMobileUnitsToPosition>(playerId, mobileUnitIds, destination));

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

        std::shared_ptr<const CompleteGameState> _completeGameState;

        std::mutex _somePlayerInputCallbackMutex;
        std::unordered_map<xg::Guid, std::function<void(std::shared_ptr<const CompleteGameState>)>> _somePlayerInputCallback;
    };
}