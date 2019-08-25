#pragma once

#include "shared/communication/messages/game/CommunicatedPlayer.h"
#include "shared/communication/messages/game/CommunicatedSinguity.h"

#include "shared/game/play/Player.h"

#include "commons/CollectionPipe.h"

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

        std::shared_ptr<Player> communicatedPlayerToPhysics(CommunicatedPlayer player, immer::vector<CommunicatedSinguity> singuities)
        {
            return std::make_shared<Player>(
                player.playerId(),
                *(std::make_shared<immer::vector<CommunicatedSinguity>>(singuities)
                    | map<std::shared_ptr<Singuity>>([this](const CommunicatedSinguity& singuity) {
                        return std::make_shared<Singuity>(
                            singuity.singuityId(),
                            communicatedVector2DToPhysics(singuity.position()),
                            communicatedVector2DToPhysics(singuity.speed()),
                            singuity.destination().map<Vector2D>(std::bind(&PhysicsCommunicationAssembler::communicatedVector2DToPhysics, this, std::placeholders::_1)),
                            singuity.isBreakingForDestination(),
                            singuity.healthPoint(),
                            singuity.lastShootTimestamp()
                        );
                    }) | toVector<std::shared_ptr<Singuity>>()
                )
            );
        }

        immer::vector<CommunicatedSinguity> physicsPlayerToCommunicatedSinguities(std::shared_ptr<const Player> player)
        {
            std::vector<CommunicatedSinguity> communicatedSinguities;
            for (const auto& singuity : *player->singuities())
            {
                communicatedSinguities.emplace_back(
                    singuity->id(),
                    player->id(),
                    physicsVector2DToCommunicated(singuity->position()),
                    physicsVector2DToCommunicated(singuity->speed()),
                    singuity->destination().map<CommunicatedVector2D>([this](const Vector2D& destination) { return physicsVector2DToCommunicated(destination); }),
                    singuity->isBreakingForDestination(),
                    singuity->healthPoints(),
                    singuity->lastShootTimestamp()
                );
            }

            return immer::vector<CommunicatedSinguity>(communicatedSinguities.begin(), communicatedSinguities.end());
        }

        Vector2D communicatedVector2DToPhysics(const CommunicatedVector2D& vector2D)
        {
            return Vector2D(vector2D.x(), vector2D.y());
        }

        CommunicatedVector2D physicsVector2DToCommunicated(const Vector2D& vector2D)
        {
            return CommunicatedVector2D(vector2D.x(), vector2D.y());
        }
    };
}