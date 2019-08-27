#pragma once

#include "PlayerClient.h"

#include "server/game/GameReceiver.h"

#include "shared/communication/MessageSerializer.h"

namespace uw
{
    class ClientsReceiver
    {
    public:

        ClientsReceiver(std::shared_ptr<MessageSerializer> messageSerializer, std::shared_ptr<GameReceiver> gameReceiver) :
            _messageSerializer(messageSerializer),
            _gameReceiver(gameReceiver)
        {}

        void addPlayerClient(const PlayerClient& playerClient)
        {
            std::lock_guard<std::mutex> lockPlayerClients(_playerClientsMutex);

            _playerClients.emplace_back(std::make_shared<PlayerClient>(playerClient));
            _clientWaiters.emplace_back(std::make_shared<std::thread>([this, playerClient] { loopClientReceive(playerClient); }));
        }

        void stop()
        {
            std::lock_guard<std::mutex> lockPlayerClients(_playerClientsMutex);

            for (auto playerClient : _playerClients)
            {
                playerClient->client()->close();
            }

            for (auto clientWaiter : _clientWaiters)
            {
                clientWaiter->join();
            }

            _playerClients.clear();
            _clientWaiters.clear();
        }

    private:

        void loopClientReceive(const PlayerClient& playerClient)
        {
            int failureCount = 0;
            while (playerClient.client()->isOpen() && failureCount < 3)
            {
                std::string receivedCommunication;
                try
                {
                    receivedCommunication = playerClient.client()->receive();
                    failureCount = 0;
                }
                catch (...)
                {
                    ++failureCount;
                }

                const auto messages(_messageSerializer->deserialize(receivedCommunication));
                _gameReceiver->receiveMessages(playerClient.playerId(), messages);
            }

            playerClient.client()->close();

            std::lock_guard<std::mutex> lockPlayerClients(_playerClientsMutex);

            int clientIndex = 0;
            while (_playerClients[clientIndex]->playerId() != playerClient.playerId()) ++clientIndex;

            if (clientIndex < _playerClients.size())
            {
                _playerClients.erase(_playerClients.begin() + clientIndex);
                _clientWaiters.erase(_clientWaiters.begin() + clientIndex);
            }
        }

        std::mutex _playerClientsMutex;
        std::vector<std::shared_ptr<std::thread>> _clientWaiters;
        std::vector<std::shared_ptr<PlayerClient>> _playerClients;
        const std::shared_ptr<MessageSerializer> _messageSerializer;
        const std::shared_ptr<GameReceiver> _gameReceiver;
    };
}