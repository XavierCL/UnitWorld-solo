#pragma once

#include "../communication/game/CommunicatedPlayer.h"
#include "../communication/game/CommunicatedSinguity.h"

#include "../game/play/Player.h"

#include <immer/vector.hpp>

namespace uw
{
    class PhysicsCommunicationAssembler
    {
    public:
        CommunicatedPlayer physicsPlayerToCommunicated(std::shared_ptr<const Player> player)
        {
            return CommunicatedPlayer(player->id());
        }

        immer::vector<CommunicatedSinguity> physicsPlayerToCommunicatedSinguities(std::shared_ptr<const Player> player)
        {
            std::vector<CommunicatedSinguity> communicatedSinguities;
            for (const auto& singuity : player->singuities())
            {
                communicatedSinguities.emplace_back(
                    singuity->id(),
                    player->id(),
                    physicsVector2DToCommunicated(singuity->position()),
                    physicsVector2DToCommunicated(singuity->speed()),
                    singuity->destination().map<CommunicatedVector2D>([this](const Vector2D& destination) { return physicsVector2DToCommunicated(destination); })
                );
            }

            return immer::vector<CommunicatedSinguity>(communicatedSinguities.begin(), communicatedSinguities.end());
        }

    private:

        CommunicatedVector2D physicsVector2DToCommunicated(const Vector2D& vector2D)
        {
            return CommunicatedVector2D(vector2D.x(), vector2D.y());
        }
    };
}