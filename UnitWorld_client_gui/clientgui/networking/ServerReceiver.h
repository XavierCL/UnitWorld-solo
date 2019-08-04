#pragma once


#include "shared/game/GameManager.h"

#include "shared/communication/MessageSerializer.h"

#include "shared/communication/messages/Message.h"
#include "shared/communication/messages/CompleteGameStateMessage.h"

#include <communications/CommunicationHandler.h>

namespace uw
{
    class ServerReceiver
    {
    public:
        ServerReceiver(std::shared_ptr<CommunicationHandler> serverHandler, std::shared_ptr<GameManager> gameManager, std::shared_ptr<MessageSerializer> messageSerializer) :
            _serverHandler(serverHandler),
            _gameManager(gameManager),
            _messageSerializer(messageSerializer)
        {}

        ~ServerReceiver()
        {
            stop();
        }

        void stop()
        {
            _serverHandler->close();

            _receiveThread.join();
        }

        void startAsync()
        {
            _receiveThread = std::thread([this] {
            
                while (_serverHandler->isOpen())
                {
                    receiveServerCommunications();
                }
            });
        }

    private:

        void receiveServerCommunications()
        {
            const auto communication = _serverHandler->receive();
            
            const auto messageWrappers = _messageSerializer->deserialize(communication);

            handleServerMessages(messageWrappers);
        }

        void handleServerMessages(const std::vector<MessageWrapper>& messageWrappers)
        {
            if (messageWrappers.empty())
            {
                return;
            }

            size_t maxTimestampIndex = 0;
            size_t messageWrapperIndex = 1;
            while (messageWrapperIndex < messageWrappers.size())
            {
                if (messageWrappers[messageWrapperIndex].timestamp() > messageWrappers[maxTimestampIndex].timestamp())
                {
                    maxTimestampIndex = messageWrapperIndex;
                }
                ++messageWrapperIndex;
            }

            handleServerMessage(messageWrappers[maxTimestampIndex].innerMessage());
        }

        void handleServerMessage(std::shared_ptr<const Message> message)
        {
            const auto completeStateMessage(std::dynamic_pointer_cast<CompleteGameStateMessage const>(message));

            std::unordered_map<xg::Guid, std::shared_ptr<Player>> completeStatePlayers;
            for (const auto& communicatedPlayer : completeStateMessage->getPlayers())
            {
                completeStatePlayers[communicatedPlayer.playerId()] = std::make_shared<Player>(communicatedPlayer.playerId(), std::vector<std::shared_ptr<Singuity>>());
            }

            for (const auto& communicatedSinguity : completeStateMessage->getSinguities())
            {
                const auto foundPlayer(completeStatePlayers.find(communicatedSinguity.playerId()));

                if (foundPlayer != completeStatePlayers.end())
                {
                    const Vector2D singuityPosition(communicatedSinguity.position().x(), communicatedSinguity.position().y());
                    const Vector2D singuitySpeed(communicatedSinguity.speed().x(), communicatedSinguity.speed().y());
                    const Option<Vector2D> singuityDestination(communicatedSinguity.destination().map<Vector2D>([](const CommunicatedVector2D& actualDestination) {
                        return Vector2D(actualDestination.x(), actualDestination.y());
                    }));

                    foundPlayer->second->addSinguity(std::make_shared<Singuity>(singuityPosition, singuitySpeed, singuityDestination));
                }
            }

            immer::vector<std::shared_ptr<Player>> nextPlayers;

            for (const auto& playerIdAndPlayer : completeStatePlayers)
            {
                nextPlayers = std::move(nextPlayers).push_back(playerIdAndPlayer.second);
            }

            _gameManager->setNextPlayers(nextPlayers);
        }

        std::thread _receiveThread;

        std::shared_ptr<CommunicationHandler> _serverHandler;
        std::shared_ptr<GameManager> _gameManager;

        std::shared_ptr<MessageSerializer> _messageSerializer;
    };
}