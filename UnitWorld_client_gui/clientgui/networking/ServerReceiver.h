#pragma once


#include "shared/game/GameManager.h"

#include "shared/communication/MessageSerializer.h"

#include "shared/communication/messages/Message.h"
#include "shared/communication/messages/CompleteGameStateMessage.h"

#include "shared/transfers/PhysicsCommunicationAssembler.h"

#include <communications/CommunicationHandler.h>

#include "commons/MemoryExtension.h"

#include "commons/Logger.hpp"

namespace uw
{
    class ServerReceiver
    {
    public:
        ServerReceiver(std::shared_ptr<CommunicationHandler> serverHandler, std::shared_ptr<GameManager> gameManager, std::shared_ptr<PhysicsCommunicationAssembler> physicsCommunicationAssembler, std::shared_ptr<MessageSerializer> messageSerializer) :
            _serverHandler(serverHandler),
            _gameManager(gameManager),
            _physicsCommunicationAssembler(physicsCommunicationAssembler),
            _messageSerializer(messageSerializer),
            _mostRecentCompleteStateTimestamp(0)
        {}

        void stop()
        {
            _serverHandler->close();

            _receiveThread.join();
        }

        void startAsync()
        {
            _receiveThread = std::thread([this] {
            
                try
                {
                    while (_serverHandler->isOpen())
                    {
                        receiveServerCommunications();
                    }
                }
                catch (std::exception error)
                {
                    Logger::error("Error while receiving from the server socket the socket is likely closed. Error message: " + std::string(error.what()));
                }
            });
        }

    private:

        void receiveServerCommunications()
        {
            const auto communication = _serverHandler->receive();
            
            const auto messageWrappers = _messageSerializer->deserialize(communication);

            for (const auto& messageWrapper : messageWrappers)
            {
                handleServerMessage(messageWrapper);
            }
        }

        void handleServerMessage(const MessageWrapper& messageWrapper)
        {
            if (messageWrapper.messageType() == MessageType::CompleteGameStateMessageType)
            {
                if (messageWrapper.timestamp() > _mostRecentCompleteStateTimestamp)
                {
                    _mostRecentCompleteStateTimestamp = messageWrapper.timestamp();
                    const auto completeStateMessage(std::dynamic_pointer_cast<CompleteGameStateMessage const>(messageWrapper.innerMessage()));
                    handleCompleteGameStateMessage(completeStateMessage);
                }
            }
        }

        void handleCompleteGameStateMessage(std::shared_ptr<CompleteGameStateMessage const> completeStateMessage)
        {
            const auto currentPlayerId = completeStateMessage->getCurrentPlayerId();

            _gameManager->setNextCompleteGameState(_physicsCommunicationAssembler->communicatedCompleteGameStateToPhysics(completeStateMessage->completeGameState()));
            _gameManager->setNextCurrentPlayerId(currentPlayerId);
        }

        std::thread _receiveThread;

        unsigned long long _mostRecentCompleteStateTimestamp;

        const std::shared_ptr<CommunicationHandler> _serverHandler;
        const std::shared_ptr<GameManager> _gameManager;

        const std::shared_ptr<PhysicsCommunicationAssembler> _physicsCommunicationAssembler;
        const std::shared_ptr<MessageSerializer> _messageSerializer;
    };
}