#pragma once

#include "shared/transfers/PhysicsCommunicationAssembler.h"

#include "shared/communication/messages/MoveMobileUnitsToPositionMessage.h"
#include "shared/communication/messages/MoveMobileUnitsToSpawnerMessage.h"
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

        void moveUnitsToPosition(const std::unordered_set<xg::Guid>& singuityIds, const Vector2D& destination)
        {
            const auto communicatedDestination(_physicsCommunicationAssembler->physicsVector2DToCommunicated(destination));

            const auto message(std::make_shared<MoveMobileUnitsToPositionMessage>(std::vector<xg::Guid>(singuityIds.begin(), singuityIds.end()), communicatedDestination));

            try
            {
                _serverCommunicator->send(_messageSerializer->serialize(std::vector<MessageWrapper> { MessageWrapper(message) }));
            }
            catch (std::exception error)
            {
                Logger::error("Error while sending command to server: " + std::string(error.what()));
            }
        }

        void moveUnitsToSpawner(const std::unordered_set<xg::Guid>& singuityIds, const xg::Guid& spawnerId)
        {
            const auto message(std::make_shared<MoveMobileUnitsToSpawnerMessage>(std::vector<xg::Guid>(singuityIds.begin(), singuityIds.end()), spawnerId));

            try
            {
                _serverCommunicator->send(_messageSerializer->serialize(std::vector<MessageWrapper> { MessageWrapper(message) }));
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