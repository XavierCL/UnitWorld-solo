#pragma once

#include "shared/game/play/CompleteGameState.h"
#include "shared/communication/messages/game/CommunicatedCompleteGameState.h"

#include "commons/CollectionPipe.h"

#include <immer/vector.hpp>

namespace uw
{
    class PhysicsCommunicationAssembler
    {
    public:
        CommunicatedCompleteGameState physicsCompleteGameStateToCommunicated(std::shared_ptr<const CompleteGameState> completeGameState) const
        {
            auto communicatedPlayers =
                &completeGameState->players()
                | map<CommunicatedPlayer>([this](std::shared_ptr<const Player> player) { return physicsPlayerToCommunicated(player); })
                | toVector<CommunicatedPlayer>();

            auto communicatedSinguities =
                &completeGameState->players()
                | flatMap<CommunicatedSinguity>([this](std::shared_ptr<const Player> player) {
                    auto singuities = physicsPlayerToCommunicatedSinguities(player);
                    return std::make_shared<immer::vector<CommunicatedSinguity>>(singuities.begin(), singuities.end());
                }) | toVector<CommunicatedSinguity>();

            auto communicatedSpawners =
                &completeGameState->spawners()
                | map<CommunicatedSpawner>([this](std::shared_ptr<Spawner> spawner) { return physicsSpawnerToCommunicated(spawner); })
                | toVector<CommunicatedSpawner>();

            return CommunicatedCompleteGameState(*communicatedPlayers, *communicatedSinguities, *communicatedSpawners);
        }

        CompleteGameState communicatedCompleteGameStateToPhysics(CommunicatedCompleteGameState&& completeGameState) const
        {
            auto spawners = &completeGameState.spawners()
                | map<std::shared_ptr<Spawner>>([this](const CommunicatedSpawner& spawner) {
                    return communicatedSpawnerToPhysics(spawner);
                }) | toVector<std::shared_ptr<Spawner>>();

            auto players = &completeGameState.players()
                | map<std::shared_ptr<Player>>([this, &completeGameState](const CommunicatedPlayer& player) {
                    auto communicatedSinguities = &completeGameState.singuities()
                        | filter<CommunicatedSinguity>([&player](const CommunicatedSinguity& singuity) {
                            return singuity.playerId() == player.playerId();
                    }) | toVector<CommunicatedSinguity>();

                    return communicatedPlayerToPhysics(player, *communicatedSinguities);
                }) | toVector<std::shared_ptr<Player>>();

            return CompleteGameState(std::move(*spawners), std::move(*players));
        }

        CommunicatedVector2D physicsVector2DToCommunicated(const Vector2D& vector2D) const
        {
            return CommunicatedVector2D(vector2D.x(), vector2D.y());
        }

        Vector2D communicatedVector2DToPhysics(const CommunicatedVector2D& vector2D) const
        {
            return Vector2D(vector2D.x(), vector2D.y());
        }

    private:

        CommunicatedPlayer physicsPlayerToCommunicated(std::shared_ptr<const Player> player) const
        {
            return CommunicatedPlayer(player->id());
        }

        std::shared_ptr<Player> communicatedPlayerToPhysics(CommunicatedPlayer player, std::vector<CommunicatedSinguity> singuities) const
        {
            auto physicsSinguities = &singuities
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
            }) | toVector<std::shared_ptr<Singuity>>();

            return std::make_shared<Player>(
                player.playerId(),
                *physicsSinguities
            );
        }

        immer::vector<CommunicatedSinguity> physicsPlayerToCommunicatedSinguities(std::shared_ptr<const Player> player) const
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
                    singuity->healthPoint(),
                    singuity->lastShootTimestamp()
                );
            }

            return immer::vector<CommunicatedSinguity>(communicatedSinguities.begin(), communicatedSinguities.end());
        }

        CommunicatedSpawner physicsSpawnerToCommunicated(std::shared_ptr<Spawner> spawner) const
        {
            return CommunicatedSpawner(
                spawner->id(),
                physicsVector2DToCommunicated(spawner->position()),
                spawner->allegence().map<CommunicatedSpawnerAllegence>([](const SpawnerAllegence& allegence) {
                    return CommunicatedSpawnerAllegence(allegence.isClaimed(), allegence.healthPoint(), allegence.allegedPlayerId());
                }),
                spawner->lastSpawnTimestamp(),
                spawner->totalSpawnedCount()
            );
        }

        std::shared_ptr<Spawner> communicatedSpawnerToPhysics(const CommunicatedSpawner& spawner) const
        {
            return std::make_shared<Spawner>(
                spawner.id(),
                communicatedVector2DToPhysics(spawner.position()),
                spawner.allegence().map<SpawnerAllegence>([](const CommunicatedSpawnerAllegence& allegence) {
                    return SpawnerAllegence(allegence.isClaimed(), allegence.healthPoint(), allegence.playerId());
                }),
                spawner.lastSpawnTimestamp(),
                spawner.totalSpawnedCount()
            );
        }
    };
}