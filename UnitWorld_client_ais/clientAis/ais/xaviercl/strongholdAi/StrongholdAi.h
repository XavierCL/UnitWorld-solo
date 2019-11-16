#pragma once

#include "clientAis/ais/Artificial.h"

#include "shared/game/physics/collisions/KdtreeCollisionDetectorFactory.h"

#include "commons/CollectionPipe.h"

namespace uw
{
    class StrongholdAi: public Artificial
    {
    public:
        StrongholdAi():
            _collisionDetectorFactory(std::make_shared<KdtreeCollisionDetectorFactory>())
        {}

        void frameHappened(std::shared_ptr<GameManager> gameManager, std::shared_ptr<ServerCommander> serverCommander)
        {
            const auto currentPlayerOpt = gameManager->currentPlayer();
            currentPlayerOpt.foreach([gameManager, serverCommander, this](const std::shared_ptr<Player> currentPlayer) {
                const auto completeGameState = gameManager->completeGameState();

                const auto satelliteSpawners = getSatelliteSpawners(completeGameState, currentPlayer);

                const auto singuitiesWithoutDestination = currentPlayer->singuities()
                    | filter<std::shared_ptr<Singuity>>([](const std::shared_ptr<Singuity> singuity) {
                        return singuity->destination().isEmpty();
                    }) | toVector<std::shared_ptr<Singuity>>();

                auto satellitesCollisionDetector = getCollisionDetectorForSpawners(satelliteSpawners);
                const auto spawnersById = &completeGameState->spawners()
                    | toUnorderedMap<xg::Guid, std::shared_ptr<Spawner>>([](const std::shared_ptr<Spawner> spawner) { return spawner->id(); });

                // destination spawner position of singuities without destination already
                std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>> movingSinguitiesBySpawnerId;
                for (const auto singuityWithoutDestination : *singuitiesWithoutDestination)
                {
                    Option<xg::Guid> closestSatellite = satellitesCollisionDetector->getClosest(CollidablePoint(singuityWithoutDestination->id(), singuityWithoutDestination->position()));
                    closestSatellite.foreach([&movingSinguitiesBySpawnerId, singuityWithoutDestination](const xg::Guid& spawnerId) {
                        movingSinguitiesBySpawnerId[spawnerId].emplace(singuityWithoutDestination->id());
                    });
                }

                auto enemySpawnerCollisionDetector = getCollisionDetectorForSpawners(*(&completeGameState->spawners()
                    | filter<std::shared_ptr<Spawner>>([currentPlayer](const std::shared_ptr<Spawner> spawner) { return spawner->canBeInteractedWithBy(currentPlayer->id()); })
                    | toVector<std::shared_ptr<Spawner>>()));

                // destination spawner of singuities close to an enemy spawner
                std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>> closeSinguitiesBySpawnerId;
                for (const auto closeSinguity : *currentPlayer->singuities())
                {
                    Option<xg::Guid> closestSpawner = enemySpawnerCollisionDetector->getClosest(CollidablePoint(closeSinguity->id(), closeSinguity->position()));
                    closestSpawner.foreach([&closeSinguitiesBySpawnerId, closeSinguity](const xg::Guid& spawnerId) {
                        closeSinguitiesBySpawnerId[spawnerId].emplace(closeSinguity->id());
                    });
                }

                std::unordered_map<xg::Guid, long long> spawnerEnemyStrongholds = getEnemySpawnerStrongholds(completeGameState, currentPlayer->id());

                // application of the singuity attacks
                std::unordered_set<xg::Guid> singuitiesWentAttacking;
                for (const auto attackingSpawner : completeGameState->spawners())
                {
                    const auto& closeSinguities = closeSinguitiesBySpawnerId[attackingSpawner->id()];
                    const auto& enemyStronghold = spawnerEnemyStrongholds[attackingSpawner->id()];
                    if (closeSinguities.size() >= enemyStronghold)
                    {
                        singuitiesWentAttacking.insert(closeSinguities.begin(), closeSinguities.end());
                        serverCommander->moveUnitsToSpawner(closeSinguities, attackingSpawner->id());
                    }
                }

                // application of the singuities without destination's movement to an ally spawner position
                for (const auto destination : movingSinguitiesBySpawnerId)
                {
                    auto singuitiesGoingForDestination = destination.second;
                    if (singuitiesGoingForDestination.size() > 0)
                    {
                        (spawnersById | find<std::shared_ptr<Spawner>>(destination.first)).foreach([&singuitiesGoingForDestination, &singuitiesWentAttacking, serverCommander](const std::shared_ptr<Spawner> closeSpawner) {
                            for (const auto singuityWentAttacking : singuitiesWentAttacking)
                            {
                                singuitiesGoingForDestination.erase(singuityWentAttacking);
                            }
                            serverCommander->moveUnitsToPosition(singuitiesGoingForDestination, closeSpawner->position());
                        });
                    }
                }
            });
        }

    private:

        std::vector<std::shared_ptr<Spawner>> getSatelliteSpawners(const std::shared_ptr<const CompleteGameState> completeGameState, const std::shared_ptr<Player> currentPlayer)
        {
            const auto& spawners = completeGameState->spawners();

            const auto spawnersById = &spawners
                | toUnorderedMap<xg::Guid, std::shared_ptr<Spawner>>([](const std::shared_ptr<Spawner> spawner) { return spawner->id(); });

            const auto ownSpawners = &spawners
                | filter<std::shared_ptr<Spawner>>([currentPlayer](const std::shared_ptr<Spawner> spawner) { return spawner->isClaimedBy(currentPlayer->id()); })
                | toVector<std::shared_ptr<Spawner>>();

            auto ownSpawnerCollisionDetector = _collisionDetectorFactory->create();

            const auto ownSpawnerCollidablePoints = ownSpawners
                | map<CollidablePoint>([](const std::shared_ptr<Spawner> spawner) { return CollidablePoint(spawner->id(), spawner->position()); })
                | toVector<CollidablePoint>();

            ownSpawnerCollisionDetector->updateAllCollidablePoints(*ownSpawnerCollidablePoints);

            std::unordered_map<xg::Guid, std::shared_ptr<Spawner>> satelliteSpawners;

            for (const auto spawner : spawners)
            {
                if (spawner->canBeInteractedWithBy(currentPlayer->id()))
                {
                    // Claimed but hurt spawners can be the closest to themselves, thus not giving the id of the spawner for collision detection
                    // NOTE: Two points with the same ID will not be detected as a collision
                    Option<xg::Guid> closestOwnSpawner = ownSpawnerCollisionDetector->getClosest(CollidablePoint(xg::Guid(), spawner->position()));

                    closestOwnSpawner.foreach([spawnersById, &satelliteSpawners](const xg::Guid& spawnerId) {
                        (spawnersById | find<std::shared_ptr<Spawner>>(spawnerId)).foreach([&satelliteSpawners](const std::shared_ptr<Spawner> spawner) {
                            satelliteSpawners.insert(std::make_pair(spawner->id(), spawner));
                        });
                    });
                }
            }

            return *(&satelliteSpawners | mapValues<std::shared_ptr<Spawner>>() | toVector<std::shared_ptr<Spawner>>());
        }

        std::shared_ptr<CollisionDetector> getCollisionDetectorForSpawners(const std::vector<std::shared_ptr<Spawner>>& spawners)
        {
            std::vector<CollidablePoint> collidablePoints {};

            for (const auto spawner : spawners)
            {
                collidablePoints.emplace_back(spawner->id(), spawner->position());
            }

            auto collisionDetector = _collisionDetectorFactory->create();
            collisionDetector->updateAllCollidablePoints(collidablePoints);

            return collisionDetector;
        }

        std::unordered_map<xg::Guid, long long> getEnemySpawnerStrongholds(std::shared_ptr<const CompleteGameState> completeGameState, const xg::Guid& currentPlayerId)
        {
            std::unordered_map<xg::Guid, long long> enemyStrongholds;

            std::shared_ptr<CollisionDetector> spawnersCollisionDetector = getCollisionDetectorForSpawners(completeGameState->spawners());

            for (const auto player : completeGameState->players())
            {
                if (player->id() == currentPlayerId) continue;

                for (const auto singuity : *player->singuities())
                {
                    const auto closestSpawner = spawnersCollisionDetector->getClosest(CollidablePoint(singuity->id(), singuity->position()));
                    closestSpawner.foreach([&enemyStrongholds](const xg::Guid& spawnerId) {
                        ++enemyStrongholds[spawnerId];
                    });
                }
            }

            for (const auto spawner : completeGameState->spawners())
            {
                Singuity referenceSinguity(Vector2D(0, 0));
                if (spawner->isAllegedToPlayer(currentPlayerId))
                {
                    enemyStrongholds[spawner->id()] += ceil((spawner->maximumHealthPoint() - spawner->healthPoint()) / referenceSinguity.reguvenatingHealth());
                }
                else
                {
                    enemyStrongholds[spawner->id()] += ceil(spawner->maximumHealthPoint() / referenceSinguity.reguvenatingHealth()) + ceil(spawner->healthPoint() / 20.0);
                }
            }

            return enemyStrongholds;
        }

        const std::shared_ptr<CollisionDetectorFactory> _collisionDetectorFactory;
    };
}