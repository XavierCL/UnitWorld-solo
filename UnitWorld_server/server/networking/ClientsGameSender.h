#pragma once

#include "PlayerClient.h"

#include "shared/game/GameManager.h"

#include "shared/transfers/PhysicsCommunicationAssembler.h"

#include "shared/communication/MessageSerializer.h"
#include "shared/communication/messages/CompleteGameStateMessage.h"

#include "shared/communication/messages/game/CommunicatedPlayer.h"
#include "shared/communication/messages/game/CommunicatedSinguity.h"

#include "communications/CommunicationHandler.h"

#include <chrono>
#include <mutex>

namespace uw
{
    class ClientsGameSender
    {
    public:
        ClientsGameSender(std::shared_ptr<GameManager> gameManager, std::shared_ptr<MessageSerializer> messageSerializer, std::shared_ptr<PhysicsCommunicationAssembler> physicsCommunicationAssembler) :
            _gameManager(gameManager),
            _messageSerializer(messageSerializer),
            _maxMsBetweenCompleteStates(60000 / MINIMAL_COMPLETE_STATE_PER_MINUTE),
            _isRunning(true),
            _lastSendCompleteState(std::chrono::steady_clock::now()),
            _physicsCommunicationAssembler(physicsCommunicationAssembler)
        {}

        void addClient(const PlayerClient& playerClient)
        {
            _playerClients = _playerClients.push_back(playerClient);
        }

        void startAsync()
        {
            _gameManager->addPlayerInputCallback(PLAYER_INPUT_GAME_MANAGER_CALLBACK_ID, std::bind(&ClientsGameSender::onGameManagerHasPlayerInput, this, std::placeholders::_1));

            _senderThread = std::thread([this] { loopSendCompleteState(); });
        }

        void stop()
        {
            _gameManager->removePlayerInputCallback(PLAYER_INPUT_GAME_MANAGER_CALLBACK_ID);

            _isRunning = false;

            _senderThread.join();
        }

    private:

        void onGameManagerHasPlayerInput(const immer::vector<std::shared_ptr<Player>>& players)
        {
            std::thread([this] { sendCompleteState(); }).detach();
        }

        void loopSendCompleteState()
        {
            while (_isRunning)
            {
                const auto currentTime = std::chrono::steady_clock::now();

                const auto msSinceLastSend = (unsigned int)std::chrono::duration<double, std::milli>(currentTime - _lastSendCompleteState).count();

                if (msSinceLastSend >= _maxMsBetweenCompleteStates)
                {
                    sendCompleteState();
                }
                else
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(_maxMsBetweenCompleteStates - msSinceLastSend));
                }
            }
        }

        void sendCompleteState()
        {
            std::lock_guard<std::mutex> sendCompleteStateLock(_sendCompleteStateMutex);

            _lastSendCompleteState = std::chrono::steady_clock::now();

            auto localPlayerClients(_playerClients);

            const auto players = _gameManager->players();

            std::vector<CommunicatedPlayer> communicatedPlayers;
            std::vector<CommunicatedSinguity> communicatedSinguities;
            for (const auto player : players)
            {
                communicatedPlayers.emplace_back(_physicsCommunicationAssembler->physicsPlayerToCommunicated(player));
                const auto assembledPlayerSinguities(_physicsCommunicationAssembler->physicsPlayerToCommunicatedSinguities(player));

                std::copy(assembledPlayerSinguities.begin(), assembledPlayerSinguities.end(), std::back_inserter(communicatedSinguities));
            }

            for (auto playerClient : localPlayerClients)
            {
                const auto message = MessageWrapper(std::make_shared<CompleteGameStateMessage>(communicatedPlayers, communicatedSinguities, playerClient.playerId()));
                playerClient.client()->send(_messageSerializer->serialize(std::vector<MessageWrapper>(1, message)));
            }
        }

        static const unsigned int MINIMAL_COMPLETE_STATE_PER_MINUTE;
        static const xg::Guid PLAYER_INPUT_GAME_MANAGER_CALLBACK_ID;

        bool _isRunning;
        std::thread _senderThread;

        immer::vector<PlayerClient> _playerClients;
        std::chrono::steady_clock::time_point _lastSendCompleteState;
        std::mutex _sendCompleteStateMutex;

        const unsigned int _maxMsBetweenCompleteStates;
        const std::shared_ptr<GameManager> _gameManager;
        const std::shared_ptr<MessageSerializer> _messageSerializer;
        const std::shared_ptr<PhysicsCommunicationAssembler> _physicsCommunicationAssembler;
    };
}