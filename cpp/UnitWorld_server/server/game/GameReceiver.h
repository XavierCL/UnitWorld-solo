#pragma once

#include "shared/game/GameManager.h"

#include "shared/communication/MessageWrapper.h"

#include "shared/communication/messages/commands/MoveMobileUnitsToPositionMessage.h"
#include "shared/communication/messages/commands/MoveMobileUnitsToSpawnerMessage.h"
#include "shared/communication/messages/commands/SetSpawnersRallyMessage.h"


#include "shared/transfers/PhysicsCommunicationAssembler.h"

namespace uw
{
    class GameReceiver
    {
    public:
        GameReceiver(std::shared_ptr<GameManager> gameManager, std::shared_ptr<PhysicsCommunicationAssembler> physicsCommunicationAssembler):
            _gameManager(gameManager),
            _physicsCommunicationAssembler(physicsCommunicationAssembler)
        {}

        void receiveMessages(xg::Guid playerId, std::vector<std::shared_ptr<MessageWrapper>> messageWrappers)
        {
            for (const auto messageWrapper : messageWrappers)
            {
                if (messageWrapper->messageType() == MessageType::MoveMobileUnitsToPositionMessageType)
                {
                    const auto moveMessage(std::dynamic_pointer_cast<const MoveMobileUnitsToPositionMessage>(messageWrapper->innerMessage()));

                    const auto singuityIds(moveMessage->singuityIds());
                    const auto communicatedDestination(moveMessage->destination());

                    const auto destination(_physicsCommunicationAssembler->communicatedVector2DToPhysics(communicatedDestination));

                    _gameManager->setNextMobileUnitsDestination(playerId, singuityIds, destination);
                }

                else if (messageWrapper->messageType() == MessageType::MoveMobileUnitsToSpawnerMessageType)
                {
                    const auto moveMessage(std::dynamic_pointer_cast<const MoveMobileUnitsToSpawnerMessage>(messageWrapper->innerMessage()));

                    const auto singuityIds(moveMessage->singuityIds());
                    const auto spawnerId(moveMessage->spawnerId());

                    _gameManager->setNextMobileUnitsSpawnerDestination(playerId, singuityIds, spawnerId);
                }

                else if (messageWrapper->messageType() == MessageType::SetSpawnersRallyMessageType)
                {
                    const auto setRallyMessage(std::dynamic_pointer_cast<const SetSpawnersRallyMessage>(messageWrapper->innerMessage()));

                    const auto spawnersId(setRallyMessage->spawnersId());
                    const auto destination(MobileUnitDestination(_physicsCommunicationAssembler->communicatedSinguityDestinationToPhysics(setRallyMessage->destination())));

                    _gameManager->setNextSpawnersRally(playerId, spawnersId, destination);
                }
            }
        }

    private:

        const std::shared_ptr<GameManager> _gameManager;
        const std::shared_ptr<PhysicsCommunicationAssembler> _physicsCommunicationAssembler;
    };
}