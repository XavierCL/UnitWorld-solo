#pragma once


#include "shared/game/GameManager.h"

#include "shared/communication/MessageSerializer.h"

#include "shared/communication/messages/Message.h"
#include "shared/communication/messages/CompleteGameStateMessage.h"

#include "shared/transfers/PhysicsCommunicationAssembler.h"

#include <communications/CommunicationHandler.h>

#include "commons/MemoryExtension.h"

namespace uw
{
    class ServerReceiver
    {
    public:
        ServerReceiver(std::shared_ptr<CommunicationHandler> serverHandler, std::shared_ptr<GameManager> gameManager, std::shared_ptr<PhysicsCommunicationAssembler> physicsCommunicationAssembler, std::shared_ptr<MessageSerializer> messageSerializer) :
            _serverHandler(serverHandler),
            _gameManager(gameManager),
            _physicsCommunicationAssembler(physicsCommunicationAssembler),
            _messageSerializer(messageSerializer)
        {}

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
            auto singuitiesByPlayerId(std::make_shared<std::vector<CommunicatedSinguity>>(completeStateMessage->getSinguities())
                | groupBy<xg::Guid, CommunicatedSinguity>([](const CommunicatedSinguity& singuity) { return singuity.playerId(); }));

            auto players(make_shared(completeStateMessage->getPlayers())
                | map<std::shared_ptr<Player>>([this, &singuitiesByPlayerId](const CommunicatedPlayer& player) {
                    auto singuities = (*singuitiesByPlayerId)[player.playerId()];
                    return _physicsCommunicationAssembler->communicatedPlayerToPhysics(player, immer::vector<CommunicatedSinguity>(singuities.begin(), singuities.end()));
                }));

            const immer::vector<std::shared_ptr<Player>> nextPlayers(players->begin(), players->end());

            _gameManager->setNextPlayers(nextPlayers);
        }

        std::thread _receiveThread;

        const std::shared_ptr<CommunicationHandler> _serverHandler;
        const std::shared_ptr<GameManager> _gameManager;

        const std::shared_ptr<PhysicsCommunicationAssembler> _physicsCommunicationAssembler;
        const std::shared_ptr<MessageSerializer> _messageSerializer;
    };
}