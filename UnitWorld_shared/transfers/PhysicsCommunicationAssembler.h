#pragma once

#include "../communication/game/CommunicatedPlayer.h"
#include "../communication/game/CommunicatedSinguity.h"

#include "../game/play/Player.h"

#include <immer/vector.hpp>

namespace uw
{
    class PhysicsCommunicationAssembler
    {
        CommunicatedPlayer physicsPlayerToCommunicated(std::shared_ptr<const Player> player)
        {

        }

        immer::vector<CommunicatedSinguity> physicsPlayersToCommunicatedSinguities(immer::vector<std::shared_ptr<const Player>> players)
        {

        }
    };
}