#pragma once

#include "clientAis/ais/Artificial.h"

#include "shared/game/physics/collisions/KdtreeCollisionDetectorFactory.h"

#include "commons/CollectionPipe.h"

namespace uw
{
    class StrongholdAi2: public Artificial
    {
    public:
        StrongholdAi2():
            _collisionDetectorFactory(std::make_shared<KdtreeCollisionDetectorFactory>())
        {}

        void frameHappened(std::shared_ptr<GameManager> gameManager, std::shared_ptr<ServerCommander> serverCommander)
        {
            const auto currentPlayerOpt = gameManager->currentPlayer();
            currentPlayerOpt.foreach([gameManager, serverCommander, this](const std::shared_ptr<Player> currentPlayer) {
                const auto completeGameState = gameManager->completeGameState();

                const std::vector<std::shared_ptr<Spawner>> satelliteSpawners = getSatelliteSpawners(completeGameState, currentPlayer);

                auto satellitesCollisionDetector = getCollisionDetectorForSpawners(satelliteSpawners);
                const auto spawnersById = &completeGameState->spawners()
                    | toUnorderedMap<xg::Guid, std::shared_ptr<Spawner>>([](const std::shared_ptr<Spawner> spawner) { return spawner->id(); });

                auto enemySpawnerCollisionDetector = getCollisionDetectorForSpawners(*(&completeGameState->spawners()
                    | filter<std::shared_ptr<Spawner>>([currentPlayer](const std::shared_ptr<Spawner> spawner) { return spawner->canBeInteractedWithBy(currentPlayer->id()); })
                    | toVector<std::shared_ptr<Spawner>>()));

                std::vector<std::shared_ptr<Spawner>> potentialCaptures = getClosestEnemySpawnerFrom(completeGameState->spawners(), satellitesCollisionDetector, *spawnersById, currentPlayer);

                std::vector<std::shared_ptr<Spawner>> currentPlayerSpawners = *(&completeGameState->spawners()
                    | filter<std::shared_ptr<Spawner>>([currentPlayer](const std::shared_ptr<Spawner> spawner) { return spawner->isClaimedBy(currentPlayer->id()); })
                    | toVector<std::shared_ptr<Spawner>>());

                std::vector<std::pair<xg::Guid, std::unordered_set<xg::Guid>>> singuitiesByAttackingPotential = getPotentialAttackers(potentialCaptures, satelliteSpawners, currentPlayerSpawners, *spawnersById, satellitesCollisionDetector, currentPlayer->singuities());
                std::unordered_map<xg::Guid, double> potentialCapturesRelativeDistances = getPotentialCapturesRelativeDistance(currentPlayer->singuities(), potentialCaptures, singuitiesByAttackingPotential);

                std::sort(singuitiesByAttackingPotential.begin(), singuitiesByAttackingPotential.end(), [&potentialCapturesRelativeDistances](std::pair<xg::Guid, std::unordered_set<xg::Guid>> first, std::pair<xg::Guid, std::unordered_set<xg::Guid>> second) {
                    return potentialCapturesRelativeDistances[first.first] < potentialCapturesRelativeDistances[second.first];
                });

                std::unordered_map<xg::Guid, long long> spawnerEnemyStrongholds = getEnemySpawnerStrongholds(completeGameState, currentPlayer->id());

                // application of the singuity attacks
                std::unordered_set<xg::Guid> singuitiesWentAttacking;
                for (auto attackingSpawnerAndSinguities : singuitiesByAttackingPotential)
                {
                    std::unordered_set<xg::Guid>& closeSinguities = attackingSpawnerAndSinguities.second;

                    for (const auto singuityWentAttacking : singuitiesWentAttacking)
                    {
                        closeSinguities.erase(singuityWentAttacking);
                    }

                    const auto& enemyStronghold = spawnerEnemyStrongholds[attackingSpawnerAndSinguities.first];
                    if (closeSinguities.size() >= enemyStronghold)
                    {
                        singuitiesWentAttacking.insert(closeSinguities.begin(), closeSinguities.end());
                        serverCommander->moveUnitsToSpawner(closeSinguities, attackingSpawnerAndSinguities.first);
                    }
                }

                const auto singuitiesWithoutDestination = currentPlayer->singuities()
                    | filter<std::shared_ptr<Singuity>>([&singuitiesWentAttacking](const std::shared_ptr<Singuity> singuity) {
                    return singuitiesWentAttacking.count(singuity->id()) == 0;
                }) | toVector<std::shared_ptr<Singuity>>();

                // destination spawner position of singuities without destination already
                std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>> movingSinguitiesBySpawnerId;
                for (const auto singuityWithoutDestination : *singuitiesWithoutDestination)
                {
                    Option<xg::Guid> closestSatellite = satellitesCollisionDetector->getClosest(CollidablePoint(singuityWithoutDestination->id(), singuityWithoutDestination->position()));
                    closestSatellite.foreach([&movingSinguitiesBySpawnerId, singuityWithoutDestination](const xg::Guid& spawnerId) {
                        movingSinguitiesBySpawnerId[spawnerId].emplace(singuityWithoutDestination->id());
                    });
                }

                // application of the singuities without destination's movement to an ally spawner position
                for (const auto destination : movingSinguitiesBySpawnerId)
                {
                    auto singuitiesGoingForDestination = destination.second;
                    if (singuitiesGoingForDestination.size() > 0)
                    {
                        (spawnersById | find<std::shared_ptr<Spawner>>(destination.first)).foreach([&singuitiesGoingForDestination, &singuitiesWentAttacking, serverCommander](const std::shared_ptr<Spawner> closeSpawner) {
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

        std::vector<std::shared_ptr<Spawner>> getClosestEnemySpawnerFrom(const std::vector<std::shared_ptr<Spawner>>& allSpawners, const std::shared_ptr<CollisionDetector> satelliteCollisionDetector, const std::unordered_map<xg::Guid, std::shared_ptr<Spawner>>& spawnersById, const std::shared_ptr<Player> currentPlayer)
        {
            std::unordered_map<xg::Guid, std::shared_ptr<Spawner>> closestEnemySpawnersBySatellite;

            for (const auto spawner : allSpawners)
            {
                if (!spawner->canBeInteractedWithBy(currentPlayer->id())) continue;

                satelliteCollisionDetector->getClosest(CollidablePoint(spawner->id(), spawner->position())).foreach([&spawner, &spawnersById, &closestEnemySpawnersBySatellite](const xg::Guid& closestSatelliteId) {

                    const std::shared_ptr<Spawner>& currentClosestEnemy = closestEnemySpawnersBySatellite[closestSatelliteId];

                    if (!currentClosestEnemy)
                    {
                        closestEnemySpawnersBySatellite[closestSatelliteId] = spawner;
                        return;
                    }

                    const std::shared_ptr<Spawner>& closestSatellite = spawnersById.at(closestSatelliteId);
                    if (closestSatellite->position().distanceSq(spawner->position()) < closestSatellite->position().distanceSq(currentClosestEnemy->position()))
                    {
                        closestEnemySpawnersBySatellite[closestSatelliteId] = spawner;
                    }
                });
            }

            return *(&closestEnemySpawnersBySatellite | mapValues<std::shared_ptr<Spawner>>() | toVector<std::shared_ptr<Spawner>>());
        }

        std::vector<std::pair<xg::Guid, std::unordered_set<xg::Guid>>> getPotentialAttackers(const std::vector<std::shared_ptr<Spawner>>& potentialCaptures, const std::vector<std::shared_ptr<Spawner>>& satelliteSpawners, const std::vector<std::shared_ptr<Spawner>>& currentPlayerSpawners, const std::unordered_map<xg::Guid, std::shared_ptr<Spawner>>& spawnersById, const std::shared_ptr<CollisionDetector> satelliteCollisionDetector, const std::shared_ptr<std::vector<std::shared_ptr<Singuity>>>& currentPlayerSinguities)
        {
            std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>> singuitiesBySpawner;

            std::vector<std::shared_ptr<Spawner>> allMatteringSpawners = potentialCaptures;
            allMatteringSpawners.insert(allMatteringSpawners.end(), currentPlayerSpawners.begin(), currentPlayerSpawners.end());
            const auto allSpawnerCollisionDetector = getCollisionDetectorForSpawners(allMatteringSpawners);
            const std::unordered_set<xg::Guid> satelliteIds = *(&satelliteSpawners | map<xg::Guid>([](const std::shared_ptr<Spawner> spawner) { return spawner->id(); }) | toUnorderedSet<xg::Guid>());

            for (const auto& singuity : *currentPlayerSinguities)
            {
                allSpawnerCollisionDetector->getClosest(CollidablePoint(singuity->id(), singuity->position())).foreach([&singuity, &satelliteIds, &singuitiesBySpawner, &spawnersById](const xg::Guid& closestSpawnerId) {
                    const auto& closestSpawner = spawnersById.at(closestSpawnerId);

                    singuitiesBySpawner[closestSpawner->id()].insert(singuity->id());
                });
            }

            std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>> singuitiesByPotentialAttacked;

            for (const auto& potentialCapture : potentialCaptures)
            {
                const auto& closeAttackingSinguities = singuitiesBySpawner[potentialCapture->id()];
                singuitiesByPotentialAttacked[potentialCapture->id()].insert(closeAttackingSinguities.begin(), closeAttackingSinguities.end());

                // this should never ignore self collision as own damaged satellites are indeed close to themselves
                satelliteCollisionDetector->getClosest(CollidablePoint(xg::Guid(), potentialCapture->position())).foreach([&potentialCapture, &singuitiesByPotentialAttacked, &singuitiesBySpawner](const xg::Guid& closestSatelliteId) {
                    const auto& attackingSinguities = singuitiesBySpawner[closestSatelliteId];
                    singuitiesByPotentialAttacked[potentialCapture->id()].insert(attackingSinguities.begin(), attackingSinguities.end());
                });
            }

            const auto potentialCapturesCollisionDetector = getCollisionDetectorForSpawners(potentialCaptures);
            for (const auto& satellite : satelliteSpawners)
            {
                potentialCapturesCollisionDetector->getClosest(CollidablePoint(xg::Guid(), satellite->position())).foreach([&satellite, &singuitiesByPotentialAttacked, &singuitiesBySpawner](const xg::Guid closestPotentialCaptureId) {
                    const auto& attackingSinguities = singuitiesBySpawner[satellite->id()];
                    singuitiesByPotentialAttacked[closestPotentialCaptureId].insert(attackingSinguities.begin(), attackingSinguities.end());
                });
            }
            
            std::vector<std::pair<xg::Guid, std::unordered_set<xg::Guid>>> potentialAttackedsAndSinguities;

            for (const auto& potentialAttackedAndSinguities : singuitiesByPotentialAttacked)
            {
                potentialAttackedsAndSinguities.emplace_back(potentialAttackedAndSinguities);
            }

            return potentialAttackedsAndSinguities;
        }

        std::unordered_map<xg::Guid, double> getPotentialCapturesRelativeDistance(const std::shared_ptr<std::vector<std::shared_ptr<Singuity>>> currentPlayerSinguities, const std::vector<std::shared_ptr<Spawner>>& potentialCaptures, const std::vector<std::pair<xg::Guid, std::unordered_set<xg::Guid>>> singuitiesByAttackingPotential)
        {
            const std::unordered_map<xg::Guid, Vector2D> singuitiesByPosition = *(currentPlayerSinguities
                | toUnorderedMap<xg::Guid, Vector2D>(
                    [](const std::shared_ptr<Singuity> singuity) { return singuity->id(); },
                    [](const std::shared_ptr<Singuity> singuity) { return singuity->position(); }
                ));

            const std::unordered_map<xg::Guid, Vector2D> potentialCapturesPositions = *(&potentialCaptures
                | toUnorderedMap<xg::Guid, Vector2D>(
                    [](const std::shared_ptr<Spawner> spawner) { return spawner->id(); },
                    [](const std::shared_ptr<Spawner> spawner) { return spawner->position(); }
                ));

            std::unordered_map<xg::Guid, double> relativeDistances;
            
            for (const auto& potentialCaptureAndAttackers : singuitiesByAttackingPotential)
            {
                const Vector2D potentialCapturePosition = potentialCapturesPositions.at(potentialCaptureAndAttackers.first);
                for (const auto& singuityId : potentialCaptureAndAttackers.second)
                {
                    relativeDistances[potentialCaptureAndAttackers.first] += potentialCapturePosition.distanceSq(singuitiesByPosition.at(singuityId));
                }

                relativeDistances[potentialCaptureAndAttackers.first] /= potentialCaptureAndAttackers.second.size();
            }

            return relativeDistances;
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
                else if (spawner->isClaimed())
                {
                    enemyStrongholds[spawner->id()] += ceil(spawner->healthPoint() / 20.0);
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