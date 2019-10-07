#pragma once

#include "PlayerClient.h"

#include "shared/game/GameManager.h"

#include "shared/transfers/PhysicsCommunicationAssembler.h"

#include "shared/communication/MessageSerializer.h"
#include "shared/communication/messages/CompleteGameStateMessage.h"

#include "shared/communication/messages/game/CommunicatedPlayer.h"
#include "shared/communication/messages/game/CommunicatedSinguity.h"

#include "communications/CommunicationHandler.h"

#include "commons/CollectionPipe.h"
#include "commons/Logger.hpp"

#include <chrono>
#include <mutex>
#include <condition_variable>

namespace uw
{
    class ClientsGameSender
    {
    public:
        ClientsGameSender(std::shared_ptr<GameManager> gameManager, std::shared_ptr<MessageSerializer> messageSerializer, std::shared_ptr<PhysicsCommunicationAssembler> physicsCommunicationAssembler) :
            _gameManager(gameManager),
            _messageSerializer(messageSerializer),
            _maxMsBetweenCompleteStates(60000 / MINIMAL_COMPLETE_STATE_PER_MINUTE),
            _minMsBetweenCompleteStates(1000 / MAXIMAL_COMPLETE_STATE_PER_SECOND),
            _hasStateChanged(false),
            _isRunning(true),
            _lastSendCompleteState(std::chrono::steady_clock::now()),
            _physicsCommunicationAssembler(physicsCommunicationAssembler),
            _lastSentGameStateVersion()
        {}

        void addClient(const PlayerClient& playerClient)
        {
            std::lock_guard<std::mutex> playerClientsModificationGuard(_updatePlayerClients);

            _playerClients = _playerClients.push_back(playerClient);
        }

        void startAsync()
        {
            _gameManager->addPlayerInputCallback(PLAYER_INPUT_GAME_MANAGER_CALLBACK_ID, std::bind(&ClientsGameSender::onGameManagerHasPlayerInput, this));
            _gameManager->addNewIndependentGameStateCallback(PLAYER_INPUT_GAME_MANAGER_CALLBACK_ID, std::bind(&ClientsGameSender::onGameManagerHasNewGameState, this));

            _senderThread = std::thread([this] { loopSendCompleteState(); });
        }

        void stop()
        {
            _gameManager->removePlayerInputCallback(PLAYER_INPUT_GAME_MANAGER_CALLBACK_ID);
            _gameManager->removeNewIndependentGameStateCallback(PLAYER_INPUT_GAME_MANAGER_CALLBACK_ID);

            {
                auto wakerLock = std::unique_lock<std::mutex>(_sendCompleteGameStateMutex);
                _isRunning = false;
                _completeStateWaker.notify_all();
            }

            _senderThread.join();
        }

    private:

        void onGameManagerHasPlayerInput()
        {
            auto wakerLock = std::unique_lock<std::mutex>(_sendCompleteGameStateMutex);
            _hasStateChanged = true;
            _completeStateWaker.notify_one();
        }

        void onGameManagerHasNewGameState()
        {
            auto wakerLock = std::unique_lock<std::mutex>(_sendCompleteGameStateMutex);
            _completeStateWaker.notify_one();
        }

        void loopSendCompleteState()
        {
            while (true)
            {
                auto wakerLock = std::unique_lock<std::mutex>(_sendCompleteGameStateMutex);

                if (!_isRunning) break;

                const auto currentTime = std::chrono::steady_clock::now();

                const auto msSinceLastSend = (unsigned int)std::chrono::duration<double, std::milli>(currentTime - _lastSendCompleteState).count();

                int msRemainingUntilNextFrame = _hasStateChanged
                    ? _minMsBetweenCompleteStates - msSinceLastSend
                    : _maxMsBetweenCompleteStates - msSinceLastSend;
                if (msRemainingUntilNextFrame <= 0)
                {
                    _hasStateChanged = false;
                    sendCompleteState();
                }
                else
                {
                    _completeStateWaker.wait(wakerLock);
                }
            }
        }

        void sendCompleteState()
        {
            _lastSendCompleteState = std::chrono::steady_clock::now();

            {
                std::lock_guard<std::mutex> playerClientsModificationGuard(_updatePlayerClients);

                const auto openClients = &_playerClients | filter<PlayerClient>([](const PlayerClient& playerClient) {
                    if (!playerClient.client()->isOpen())
                    {
                        Logger::info("client " + playerClient.client()->prettyName() + " was no longer open while sending complete state, removing it from the sending list");
                        return false;
                    }
                    else
                    {
                        return true;
                    }
                    return playerClient.client()->isOpen();
                }) | toVector<PlayerClient>();

                _playerClients = immer::vector<PlayerClient>(openClients->begin(), openClients->end());
            }

            auto localPlayerClients(_playerClients);

            const auto completeGameState = _gameManager->independentCompleteGameState();
            if (completeGameState->version() == _lastSentGameStateVersion) return;
            _lastSentGameStateVersion = completeGameState->version();

            CommunicatedCompleteGameState communicatedCompleteGameState(_physicsCommunicationAssembler->physicsCompleteGameStateToCommunicated(completeGameState));

            for (auto playerClient : localPlayerClients)
            {
                const auto message = MessageWrapper(std::make_shared<CompleteGameStateMessage>(communicatedCompleteGameState, playerClient.playerId()));
                try
                {
                    playerClient.client()->send(_messageSerializer->serialize(std::vector<MessageWrapper>(1, message)));
                }
                catch (std::exception& error)
                {
                    Logger::error("Error while sending complete state to client: " + std::string(error.what()));
                }
            }
        }

        static const unsigned int MINIMAL_COMPLETE_STATE_PER_MINUTE;
        static const unsigned int MAXIMAL_COMPLETE_STATE_PER_SECOND;
        static const xg::Guid PLAYER_INPUT_GAME_MANAGER_CALLBACK_ID;

        bool _isRunning;
        std::thread _senderThread;

        immer::vector<PlayerClient> _playerClients;
        std::chrono::steady_clock::time_point _lastSendCompleteState;
        std::mutex _updatePlayerClients;
        std::mutex _sendCompleteGameStateMutex;
        std::condition_variable _completeStateWaker;
        std::atomic<bool> _hasStateChanged;
        xg::Guid _lastSentGameStateVersion;
        
        const unsigned int _maxMsBetweenCompleteStates;
        const unsigned int _minMsBetweenCompleteStates;
        const std::shared_ptr<GameManager> _gameManager;
        const std::shared_ptr<MessageSerializer> _messageSerializer;
        const std::shared_ptr<PhysicsCommunicationAssembler> _physicsCommunicationAssembler;
    };
}