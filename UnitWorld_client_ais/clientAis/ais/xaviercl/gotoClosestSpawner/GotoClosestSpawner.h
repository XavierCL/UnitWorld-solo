#pragma once

#include "clientAis/ais/Artificial.h"

#include "shared/game/physics/collisions/KdtreeCollisionDetectorFactory.h"

#include "commons/CollectionPipe.h"

namespace uw
{
    class GotoClosestSpawner: public Artificial
    {
    public:
        GotoClosestSpawner():
            _collisionDetectorFactory(std::make_shared<KdtreeCollisionDetectorFactory>())
        {}

        void frameHappened(std::shared_ptr<GameManager> gameManager, std::shared_ptr<ServerCommander> serverCommander)
        {
            const auto currentPlayerOpt = gameManager->currentPlayer();
            currentPlayerOpt.foreach([gameManager, serverCommander, this](const std::shared_ptr<Player> currentPlayer) {
                const auto interractableSpawners = &gameManager->completeGameState()->spawners() | filter<std::shared_ptr<Spawner>>([currentPlayer](const std::shared_ptr<Spawner> spawner) {
                    return spawner->canBeInteractedWithBy(currentPlayer->id());
                }) | toVector<std::shared_ptr<Spawner>>();

                const auto collidableSpawners = interractableSpawners | map<CollidablePoint>([](const std::shared_ptr<Spawner> spawner) {
                    return CollidablePoint(spawner->id(), spawner->position());
                }) | toVector<CollidablePoint>();

                auto collisionDetector = _collisionDetectorFactory->create();

                collisionDetector->updateAllCollidablePoints(*collidableSpawners);

                // we don't want to assign a destination to a spawner that's already going to the right destination
                std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>> destinations;
                currentPlayer->singuities() | forEach([&destinations, &collisionDetector](const std::shared_ptr<Singuity> singuity) {
                    const auto closestSpawnerIdOpt = collisionDetector->getClosest(CollidablePoint(singuity->id(), singuity->position()));

                    closestSpawnerIdOpt.foreach([&destinations, &singuity](const xg::Guid closestSpawnerId) {

                        bool singuityDestinationIsAlreadySpawner = singuity->destination()
                            .map<bool>([&closestSpawnerId](const auto destination) {
                                return std::visit(overloaded{
                                    [&closestSpawnerId](const SpawnerDestination& spawnerDestination) { return spawnerDestination.spawnerId() == closestSpawnerId; },
                                    [](const Vector2D& positionDestination) { return false; }
                                }, destination);
                            }).getOrElse(false);

                        if (!singuityDestinationIsAlreadySpawner)
                        {
                            destinations[closestSpawnerId].emplace(singuity->id());
                        }
                    });
                });

                for (const auto destination : destinations)
                {
                    serverCommander->moveUnitsToSpawner(destination.second, destination.first);
                }
            });
        }

    private:
        const std::shared_ptr<CollisionDetectorFactory> _collisionDetectorFactory;
    };
}