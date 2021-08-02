#pragma once

#include "clientAis/ais/Artificial.h"

#include "shared/game/physics/collisions/KdtreeCollisionDetectorFactory.h"

#include "commons/CollectionPipe.h"

namespace uw
{
    class SpearAi: public Artificial
    {
    public:
        SpearAi():
            _collisionDetectorFactory(std::make_shared<KdtreeCollisionDetectorFactory>())
        {}

        void frameHappened(std::shared_ptr<GameManager> gameManager, std::shared_ptr<ServerCommander> serverCommander)
        {
            try
            {
                const auto currentPlayerOpt = gameManager->currentPlayer();
                currentPlayerOpt.foreach([gameManager, serverCommander, this](const std::shared_ptr<Player> currentPlayer) {
                    const auto completeGameState = gameManager->completeGameState();

                    const std::unordered_map<xg::Guid, std::shared_ptr<Spawner>> spawnersById = getSpawnersById(completeGameState);
                    const std::unordered_map<xg::Guid, std::shared_ptr<Singuity>> singuityById = getSinguityById(completeGameState);

                    const std::unordered_map<xg::Guid, long long> spawnerStrongholds = getEnemySpawnerStrongholds(completeGameState, currentPlayer->id());
                    const std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>> directNeighbourSpawners = getDirectNeighbourSpawners(spawnersById);

                    const std::unordered_set<xg::Guid> satellites = getSatellites(spawnerStrongholds, directNeighbourSpawners, spawnersById, currentPlayer->id());

                    const Vector2D averageSinguitiesPosition = getAverageSinguitiesPosition(currentPlayer->singuities());
                    const std::shared_ptr<Spawner> weakestThenClosestAttackableSpawner = getWeakestAttackableSpawner(satellites, spawnerStrongholds, directNeighbourSpawners, spawnersById, currentPlayer->id(), currentPlayer->singuities(), averageSinguitiesPosition);
                    const std::shared_ptr<Spawner> closestSatelliteToWeakestAttackableSpawner = getClosestFrom(satellites, weakestThenClosestAttackableSpawner, spawnersById, averageSinguitiesPosition);

                    const std::shared_ptr<CollisionDetector> spawnerCollisionDetector = getCollisionDetectorForSpawners(completeGameState->spawners());
                    std::unordered_set<xg::Guid> singuitiesWentAttacking = getSinguitiesClosestToSpawners(currentPlayer->singuities(), weakestThenClosestAttackableSpawner->id(), closestSatelliteToWeakestAttackableSpawner->id(), spawnerCollisionDetector);

                    if (((long long)singuitiesWentAttacking.size()) + spawnerStrongholds.at(weakestThenClosestAttackableSpawner->id()) >= 0)
                    {
                        serverCommander->moveUnitsToSpawner(singuitiesWentAttacking, weakestThenClosestAttackableSpawner->id());
                    }
                    else
                    {
                        singuitiesWentAttacking.clear();
                    }

                    std::unordered_set<xg::Guid> singuitiesGoingToSatellite;
                    for (const auto singuity : *currentPlayer->singuities())
                    {
                        if (singuitiesWentAttacking.count(singuity->id()) == 0 && !singuity->hasSpawnerDestination())
                        {
                            singuitiesGoingToSatellite.emplace(singuity->id());
                        }
                    }

                    serverCommander->moveUnitsToPosition(singuitiesGoingToSatellite, closestSatelliteToWeakestAttackableSpawner->position());
                });
            }
            catch (std::exception& e)
            {
                Logger::trace("Error while evaluating SpearAi: " + std::string(e.what()));
            }
            catch (...)
            {
                Logger::trace("Unkonwn error while evaluating SpearAi");
            }
        }

    private:

        std::unordered_map<xg::Guid, std::shared_ptr<Spawner>> getSpawnersById(const std::shared_ptr<const CompleteGameState> completeGameState)
        {
            return *(&completeGameState->spawners() | toUnorderedMap<xg::Guid, std::shared_ptr<Spawner>>([](const std::shared_ptr<Spawner> spawner) { return spawner->id(); }));
        }

        std::unordered_map<xg::Guid, std::shared_ptr<Singuity>> getSinguityById(const std::shared_ptr<const CompleteGameState> completeGameState)
        {
            return *(&completeGameState->players()
                | flatMap<std::shared_ptr<Singuity>>([](const std::shared_ptr<Player> player) { return player->singuities(); })
                | toUnorderedMap<xg::Guid, std::shared_ptr<Singuity>>([](const std::shared_ptr<Singuity> singuity) { return singuity->id(); }));
        }

        std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>> getDirectNeighbourSpawners(const std::unordered_map<xg::Guid, std::shared_ptr<Spawner>>& spawnerById)
        {
            std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>> neighbours;
            std::vector<std::shared_ptr<Spawner>> allSpawners;

            for (const auto& idAndSpawner : spawnerById)
            {
                allSpawners.emplace_back(idAndSpawner.second);
            }

            std::shared_ptr<CollisionDetector> allSpawnersCollisionDetector = getCollisionDetectorForSpawners(allSpawners);

            for (const auto& spawnerFindingNeighbours : allSpawners)
            {
                // Creating the empty neighboorhood
                neighbours[spawnerFindingNeighbours->id()];
                for (const auto& isNeighbourSpawner : allSpawners)
                {
                    Vector2D slope = isNeighbourSpawner->position() - spawnerFindingNeighbours->position();
                    slope = slope.atModule(22.0);
                    Vector2D testingPoint = spawnerFindingNeighbours->position();
                    while (true)
                    {
                        Option<xg::Guid> closestSpawnerIdOpt = allSpawnersCollisionDetector->getClosest(CollidablePoint(xg::Guid(), testingPoint));

                        if (closestSpawnerIdOpt.isEmpty())
                        {
                            break;
                        }
                        else
                        {
                            xg::Guid closestSpawnerId = closestSpawnerIdOpt.getOrThrow();

                            if (isNeighbourSpawner->id() == closestSpawnerId)
                            {
                                neighbours[spawnerFindingNeighbours->id()].emplace(closestSpawnerId);
                            }

                            const auto closestSpawner = spawnerById.at(closestSpawnerId);

                            if (closestSpawnerId != spawnerFindingNeighbours->id())
                            {
                                break;
                            }

                            if (slope.x() != 0)
                            {
                                if (slope.x() >= 0 && testingPoint.x() >= isNeighbourSpawner->position().x()
                                    || slope.x() <= 0 && testingPoint.x() <= isNeighbourSpawner->position().x())
                                {
                                    break;
                                }
                            }
                            else if (slope.y() != 0)
                            {
                                if (slope.y() >= 0 && testingPoint.y() >= isNeighbourSpawner->position().y()
                                    || slope.y() <= 0 && testingPoint.y() <= isNeighbourSpawner->position().y())
                                {
                                    break;
                                }
                            }
                            else
                            {
                                break;
                            }

                            testingPoint += slope;
                        }
                    }
                }

                neighbours[spawnerFindingNeighbours->id()].erase(spawnerFindingNeighbours->id());
            }

            return neighbours;
        }

        Vector2D getAverageSinguitiesPosition(const std::shared_ptr<std::vector<std::shared_ptr<Singuity>>> singuities)
        {
            Vector2D singuitiesAveragePosition;
            for (const auto& currentPlayerSinguity : *singuities)
            {
                singuitiesAveragePosition += currentPlayerSinguity->position();
            }
            singuitiesAveragePosition = singuitiesAveragePosition / singuities->size();

            return singuitiesAveragePosition;
        }

        std::shared_ptr<Spawner> getWeakestAttackableSpawner(const std::unordered_set<xg::Guid>& satellites, const std::unordered_map<xg::Guid, long long>& spawnerStrongholds, const std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>>& directNeighbourSpawners, const std::unordered_map<xg::Guid, std::shared_ptr<Spawner>>& spawnersById, const xg::Guid& currentPlayerId, const std::shared_ptr<std::vector<std::shared_ptr<Singuity>>> currentPlayerSinguities, const Vector2D& averageSinguitiesPosition)
        {
            std::unordered_set<xg::Guid> attackableSpawners;

            for (const auto& satelliteId : satellites)
            {
                const auto& satelliteNeighbours = directNeighbourSpawners.at(satelliteId);
                for (const auto& satelliteNeighbourId : satelliteNeighbours)
                {
                    if (spawnersById.at(satelliteNeighbourId)->canBeInteractedWithBy(currentPlayerId))
                    {
                        attackableSpawners.emplace(satelliteNeighbourId);
                    }
                }
            }

            Option<std::shared_ptr<Spawner>> closestAttackableSpawnerDestination;
            for (const auto& spawnerIdAndSpawner : spawnersById)
            {
                const auto& spawner = spawnerIdAndSpawner.second;
                
                if (attackableSpawners.count(spawner->id()) == 0) continue;

                closestAttackableSpawnerDestination = closestAttackableSpawnerDestination
                    .map<std::shared_ptr<Spawner>>([spawner, &averageSinguitiesPosition, this](const std::shared_ptr<Spawner> closestSpawner) {
                        double closestSpawnerDistanceToSinguities = closestSpawner->position().distanceSq(averageSinguitiesPosition);
                        double spawnerDistanceToSinguities = spawner->position().distanceSq(averageSinguitiesPosition);

                        if (closestSpawner->isClaimed()) closestSpawnerDistanceToSinguities /= acceptableClosestSpawnerRatio() * acceptableClosestSpawnerRatio();
                        if (spawner->isClaimed()) spawnerDistanceToSinguities /= acceptableClosestSpawnerRatio() * acceptableClosestSpawnerRatio();

                        return closestSpawnerDistanceToSinguities <= spawnerDistanceToSinguities
                            ? closestSpawner
                            : spawner;
                    }).orElse(Options::Some(spawner));
            }

            return closestAttackableSpawnerDestination.getOrThrow();
        }

        std::shared_ptr<Spawner> getClosestFrom(const std::unordered_set<xg::Guid>& satellites, const std::shared_ptr<Spawner> weakestThenClosestAttackableSpawner, const std::unordered_map<xg::Guid, std::shared_ptr<Spawner>>& spawnersById, const Vector2D& averageSinguitiesPosition)
        {
            std::shared_ptr<Spawner> closestSatellite = spawnersById.at(*satellites.begin());

            for (const auto& satelliteId : satellites)
            {
                const auto satellite = spawnersById.at(satelliteId);

                if (satellite->position().distanceSq(weakestThenClosestAttackableSpawner->position()) < closestSatellite->position().distanceSq(weakestThenClosestAttackableSpawner->position()))
                {
                    closestSatellite = satellite;
                }
                else if (satellite->position().distanceSq(weakestThenClosestAttackableSpawner->position()) == closestSatellite->position().distanceSq(weakestThenClosestAttackableSpawner->position())
                    && satellite->position().distanceSq(averageSinguitiesPosition) < closestSatellite->position().distanceSq(averageSinguitiesPosition))
                {
                    closestSatellite = satellite;
                }
            }

            return closestSatellite;
        }

        std::unordered_set<xg::Guid> getSinguitiesClosestToSpawners(const std::shared_ptr<std::vector<std::shared_ptr<Singuity>>> currentPlayerSinguities, const xg::Guid& firstSpawner, const xg::Guid& secondSpawner, const std::shared_ptr<CollisionDetector> spawnerCollisionDetector)
        {
            std::unordered_set<xg::Guid> singuitiesWentAttacking;

            for (const auto singuity : *currentPlayerSinguities)
            {
                const auto singuityClosestSpawnerId = spawnerCollisionDetector->getClosest(CollidablePoint(xg::Guid(), singuity->position()));
                singuityClosestSpawnerId.foreach([singuity, &singuitiesWentAttacking, &firstSpawner, &secondSpawner](const xg::Guid& closestSpawnerId) {
                    if (closestSpawnerId == firstSpawner || closestSpawnerId == secondSpawner)
                    {
                        singuitiesWentAttacking.emplace(singuity->id());
                    }
                });
            }

            return singuitiesWentAttacking;
        }

        std::unordered_set<xg::Guid> getSatellites(const std::unordered_map<xg::Guid, long long>& strongholds, const std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>>& neighbours, const std::unordered_map<xg::Guid, std::shared_ptr<Spawner>>& spawnersById, const xg::Guid& currentPlayerId)
        {
            std::unordered_set<xg::Guid> satellites;

            for (const auto& strongholdIdAndValue : strongholds)
            {
                if (strongholdIdAndValue.second < 0)
                {
                    for (const auto& neighbourId : neighbours.at(strongholdIdAndValue.first))
                    {
                        const auto neighbourSpawner = spawnersById.at(neighbourId);
                        if (strongholds.at(neighbourId) >= 0 && neighbourSpawner->isClaimedBy(currentPlayerId))
                        {
                            satellites.emplace(neighbourId);
                        }
                    }
                }
            }

            if (satellites.empty())
            {
                for (const auto& strongholdIdAndValue : strongholds)
                {
                    if (spawnersById.at(strongholdIdAndValue.first)->canBeInteractedWithBy(currentPlayerId))
                    {
                        for (const auto& neighbourId : neighbours.at(strongholdIdAndValue.first))
                        {
                            if (strongholds.at(neighbourId) >= 0)
                            {
                                satellites.emplace(neighbourId);
                            }
                        }
                    }
                }
            }

            return satellites;
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

        std::shared_ptr<CollisionDetector> getCollisionDetectorForSpawnersId(const std::unordered_set<xg::Guid>& spawnersId, const std::unordered_map<xg::Guid, std::shared_ptr<Spawner>>& spawnersById)
        {
            std::vector<CollidablePoint> collidablePoints{};

            for (const auto spawnerId : spawnersId)
            {
                collidablePoints.emplace_back(spawnersById.at(spawnerId)->id(), spawnersById.at(spawnerId)->position());
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
                if (player->id() == currentPlayerId)
                {
                    for (const auto singuity : *player->singuities())
                    {
                        const auto closestSpawner = spawnersCollisionDetector->getClosest(CollidablePoint(singuity->id(), singuity->position()));
                        closestSpawner.foreach([&enemyStrongholds](const xg::Guid& spawnerId) {
                            ++enemyStrongholds[spawnerId];
                        });
                    }
                }
                else
                {
                    for (const auto singuity : *player->singuities())
                    {
                        const auto closestSpawner = spawnersCollisionDetector->getClosest(CollidablePoint(singuity->id(), singuity->position()));
                        closestSpawner.foreach([&enemyStrongholds](const xg::Guid& spawnerId) {
                            --enemyStrongholds[spawnerId];
                        });
                    }
                }
            }

            for (const auto spawner : completeGameState->spawners())
            {
                Singuity referenceSinguity(Vector2D(0, 0));
                const double hundreadthOfSpawnerMaxHealth = spawner->maximumHealthPoint() / 90.0;
                if (spawner->isClaimedBy(currentPlayerId))
                {
                    enemyStrongholds[spawner->id()] += ceil(spawner->healthPoint() / hundreadthOfSpawnerMaxHealth);
                }
                if (spawner->isAllegedToPlayer(currentPlayerId))
                {
                    enemyStrongholds[spawner->id()] -= ceil((spawner->maximumHealthPoint() - spawner->healthPoint()) / referenceSinguity.reguvenatingHealth());
                }
                else if (spawner->isClaimed())
                {
                    enemyStrongholds[spawner->id()] -= ceil(spawner->healthPoint() / hundreadthOfSpawnerMaxHealth);
                }
                else
                {
                    // Spawner is alleged to enemy player or neutral
                    enemyStrongholds[spawner->id()] -= ceil(spawner->maximumHealthPoint() / referenceSinguity.reguvenatingHealth()) + ceil(spawner->healthPoint() / hundreadthOfSpawnerMaxHealth);
                }
            }

            return enemyStrongholds;
        }

        const std::shared_ptr<CollisionDetectorFactory> _collisionDetectorFactory;
        const double acceptableClosestSpawnerRatio()
        {
            return 1.5;
        }
    };
}