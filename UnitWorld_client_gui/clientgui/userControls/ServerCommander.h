#pragma once

#include "shared/transfers/PhysicsCommunicationAssembler.h"

#include "shared/communication/messages/MoveMobileUnitsToPositionMessage.h"
#include "shared/communication/MessageSerializer.h"

#include "shared/game/geometry/Vector2D.h"

#include "communications/CommunicationHandler.h"

#include "commons/Guid.hpp"
#include "commons/Logger.hpp"

#include <vector>

namespace uw
{
    class ServerCommander
    {
    public:
        ServerCommander(std::shared_ptr<CommunicationHandler> serverCommunicator, std::shared_ptr<PhysicsCommunicationAssembler> physicsCommunicationAssembler, std::shared_ptr<MessageSerializer> messageSerializer) :
            _serverCommunicator(serverCommunicator),
            _physicsCommunicationAssembler(physicsCommunicationAssembler),
            _messageSerializer(messageSerializer)
        {}

        void moveUnitsToPosition(const std::vector<xg::Guid>& singuityIds, const Vector2D& destination)
        {
            const auto communicatedDestination(_physicsCommunicationAssembler->physicsVector2DToCommunicated(destination));

            const auto message(std::make_shared<MoveMobileUnitsToPositionMessage>(singuityIds, communicatedDestination));

            try
            {
                const auto serializedMessages(_messageSerializer->serialize(std::vector<MessageWrapper> { MessageWrapper(message) }));
                _serverCommunicator->send(serializedMessages);
            }
            catch (std::exception error)
            {
                Logger::error("Error while sending command to server: " + std::string(error.what()));
            }
        }

    private:

        const std::shared_ptr<CommunicationHandler> _serverCommunicator;
        const std::shared_ptr<PhysicsCommunicationAssembler> _physicsCommunicationAssembler;
        const std::shared_ptr<MessageSerializer> _messageSerializer;
    };
}