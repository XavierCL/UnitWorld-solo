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

            _playerClients.push_back(playerClient);
            _clientWaiters.emplace_back(std::make_shared<std::thread>([this, &playerClient] { loopClientReceive(playerClient); }));
        }

        void stop()
        {
            std::lock_guard<std::mutex> lockPlayerClients(_playerClientsMutex);

            for (auto playerClient : _playerClients)
            {
                playerClient.client()->close();
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
            while (playerClient.client()->isOpen())
            {
                const auto receivedCommunication(playerClient.client()->receive());
                const auto messages(_messageSerializer->deserialize(receivedCommunication));

                _gameReceiver->receiveMessages(playerClient.playerId(), messages);
            }
        }

        std::mutex _playerClientsMutex;
        std::vector<std::shared_ptr<std::thread>> _clientWaiters;
        std::vector<PlayerClient> _playerClients;
        const std::shared_ptr<MessageSerializer> _messageSerializer;
        const std::shared_ptr<GameReceiver> _gameReceiver;
    };
}