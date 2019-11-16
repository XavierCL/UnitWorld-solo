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

            return CommunicatedCompleteGameState(*communicatedPlayers, *communicatedSinguities, *communicatedSpawners, completeGameState->frameCount());
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

            return CompleteGameState(std::move(*spawners), std::move(*players), completeGameState.frameCount());
        }

        MobileUnitDestination communicatedSinguityDestinationToPhysics(const CommunicatedSinguityDestination& singuityDestination) const
        {
            return std::visit(overloaded{
                [this](const CommunicatedVector2D& point) {
                    return MobileUnitDestination(communicatedVector2DToPhysics(point));
                },
                [this](const CommunicatedSpawnerDestination& spawnerDestination) {
                    return MobileUnitDestination(SpawnerDestination(
                        spawnerDestination.spawnerId(),
                        spawnerDestination.spawnerAllegence().map<SpawnerAllegence>([this](const CommunicatedSpawnerAllegence& allegence) { return communicatedSpawnerAllegenceToPhysics(allegence); })
                    ));
                },
                [this](const xg::Guid& unconditionalSpawnerDestination) {
                    return MobileUnitDestination(unconditionalSpawnerDestination);
                }
                }, singuityDestination.destination());
        }

        CommunicatedSinguityDestination physicsSinguityDestinationToCommunicated(const MobileUnitDestination& singuityDestination) const
        {
            return singuityDestination.map<CommunicatedSinguityDestination>(
                [this](const Vector2D& point) {
                    return CommunicatedSinguityDestination(physicsVector2DToCommunicated(point));
                },
                [this](const SpawnerDestination& spawnerDestination) {
                    return CommunicatedSinguityDestination(
                        CommunicatedSpawnerDestination(
                            spawnerDestination.spawnerId(),
                            spawnerDestination.spawnerAllegence().map<CommunicatedSpawnerAllegence>([this](const SpawnerAllegence& allegence) {
                                return physicsSpawnerAllegenceToCommunicated(allegence);
                            })
                        )
                    );
                },
                [this](const xg::Guid& unconditionalSpawnerDestination) {
                    return CommunicatedSinguityDestination(unconditionalSpawnerDestination);
                }
            );
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
                    singuity.destination().map<MobileUnitDestination>(std::bind(&PhysicsCommunicationAssembler::communicatedSinguityDestinationToPhysics, this, std::placeholders::_1)),
                    singuity.healthPoint(),
                    singuity.lastShootFrameCount()
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
                    singuity->destination().map<CommunicatedSinguityDestination>([this](const MobileUnitDestination& destination) { return physicsSinguityDestinationToCommunicated(destination); }),
                    singuity->healthPoint(),
                    singuity->lastShootFrameCount()
                );
            }

            return immer::vector<CommunicatedSinguity>(communicatedSinguities.begin(), communicatedSinguities.end());
        }

        CommunicatedSpawner physicsSpawnerToCommunicated(std::shared_ptr<Spawner> spawner) const
        {
            return CommunicatedSpawner(
                spawner->id(),
                physicsVector2DToCommunicated(spawner->position()),
                spawner->allegence().map<CommunicatedSpawnerAllegence>([this](const SpawnerAllegence& allegence) {
                    return physicsSpawnerAllegenceToCommunicated(allegence);
                }),
                spawner->rally().map<CommunicatedSinguityDestination>(std::bind(&PhysicsCommunicationAssembler::physicsSinguityDestinationToCommunicated, this, std::placeholders::_1)),
                spawner->lastSpawnFrameCount(),
                spawner->totalSpawnedCount()
            );
        }

        std::shared_ptr<Spawner> communicatedSpawnerToPhysics(const CommunicatedSpawner& spawner) const
        {
            return std::make_shared<Spawner>(
                spawner.id(),
                communicatedVector2DToPhysics(spawner.position()),
                spawner.allegence().map<SpawnerAllegence>([this](const CommunicatedSpawnerAllegence& allegence) {
                    return communicatedSpawnerAllegenceToPhysics(allegence);
                }),
                spawner.rally().map<MobileUnitDestination>(std::bind(&PhysicsCommunicationAssembler::communicatedSinguityDestinationToPhysics, this, std::placeholders::_1)),
                spawner.lastSpawnFrameCount(),
                spawner.totalSpawnedCount()
            );
        }

        CommunicatedSpawnerAllegence physicsSpawnerAllegenceToCommunicated(const SpawnerAllegence& allegence) const
        {
            return CommunicatedSpawnerAllegence(allegence.isClaimed(), allegence.healthPoint(), allegence.allegedPlayerId());
        }

        SpawnerAllegence communicatedSpawnerAllegenceToPhysics(const CommunicatedSpawnerAllegence& allegence) const
        {
            return SpawnerAllegence(allegence.isClaimed(), allegence.healthPoint(), allegence.playerId());
        }
    };
}