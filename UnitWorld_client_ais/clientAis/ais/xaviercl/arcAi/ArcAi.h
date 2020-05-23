#pragma once

#include "clientAis/ais/Artificial.h"

#include "shared/game/physics/collisions/KdtreeCollisionDetectorFactory.h"

#include "commons/CollectionPipe.h"

namespace uw
{
    class ArcAi: public Artificial
    {
    public:
        ArcAi():
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
                    //const std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>> neighbourSpawners = getNeighbourSpawners(spawnerStrongholds, spawnersById);
                    const std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>> directNeighbourSpawners = getDirectNeighbourSpawners(spawnersById);
                    //const std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>> reverseNeighbourSpawners = reverseMap(neighbourSpawners);

                    const std::unordered_set<xg::Guid> satellites = getSatellites(spawnerStrongholds, directNeighbourSpawners, spawnersById, currentPlayer->id());

                    // application of the singuity attacks
                    std::unordered_set<xg::Guid> singuitiesWentAttacking;
                    std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>> attackersByStronghold = getAttackersByStrongholds(completeGameState, currentPlayer, singuitiesWentAttacking);
                    std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>> neighbourAttackersByStronghold = getNeighbourAttackersByStrongholds(satellites, directNeighbourSpawners, attackersByStronghold, spawnerStrongholds, singuityById);
                    std::unordered_map<xg::Guid, long long> spawnerStrongholdsNeighbourRelative = getNeighbourRelativeStrongholds(spawnerStrongholds, neighbourAttackersByStronghold);
                    std::unordered_map<xg::Guid, double> spawnerStrongholdsDistanceToAverageAttackers = getNeighbourRelativeDistanceToSpawners(neighbourAttackersByStronghold, spawnersById, singuityById);
                    Option<xg::Guid> easiestTakableStronghold = getEasiestTakableStronghold(spawnerStrongholds, spawnerStrongholdsNeighbourRelative, spawnerStrongholdsDistanceToAverageAttackers, neighbourAttackersByStronghold, spawnersById, currentPlayer->id());
                    while (easiestTakableStronghold.isDefined())
                    {
                        xg::Guid takableStronghold = easiestTakableStronghold.getOrThrow();
                        const std::unordered_set<xg::Guid>& attackingSinguities = neighbourAttackersByStronghold[takableStronghold];
                        std::unordered_set<xg::Guid> sendingSinguities;

                        for (const auto& neighbourCloseSinguity : attackingSinguities)
                        {
                            if (!singuityById.at(neighbourCloseSinguity)->hasSpawnerDestination())
                            {
                                sendingSinguities.emplace(neighbourCloseSinguity);
                            }
                        }

                        singuitiesWentAttacking.insert(attackingSinguities.begin(), attackingSinguities.end());
                        serverCommander->moveUnitsToSpawner(sendingSinguities, takableStronghold);

                        attackersByStronghold = getAttackersByStrongholds(completeGameState, currentPlayer, singuitiesWentAttacking);
                        neighbourAttackersByStronghold = getNeighbourAttackersByStrongholds(satellites, directNeighbourSpawners, attackersByStronghold, spawnerStrongholds, singuityById);
                        spawnerStrongholdsNeighbourRelative = getNeighbourRelativeStrongholds(spawnerStrongholds, neighbourAttackersByStronghold);
                        spawnerStrongholdsDistanceToAverageAttackers = getNeighbourRelativeDistanceToSpawners(neighbourAttackersByStronghold, spawnersById, singuityById);
                        easiestTakableStronghold = getEasiestTakableStronghold(spawnerStrongholds, spawnerStrongholdsNeighbourRelative, spawnerStrongholdsDistanceToAverageAttackers, neighbourAttackersByStronghold, spawnersById, currentPlayer->id());
                    }

                    const auto singuitiesWithoutDestination = currentPlayer->singuities()
                        | filter<std::shared_ptr<Singuity>>([&singuitiesWentAttacking](const std::shared_ptr<Singuity> singuity) {
                        return singuitiesWentAttacking.count(singuity->id()) == 0;
                    }) | toVector<std::shared_ptr<Singuity>>();

                    const std::shared_ptr<CollisionDetector> satellitesCollisionDetector = getCollisionDetectorForSpawnersId(satellites, spawnersById);

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
                            (&spawnersById | find<std::shared_ptr<Spawner>>(destination.first)).foreach([&singuitiesGoingForDestination, &singuitiesWentAttacking, serverCommander](const std::shared_ptr<Spawner> closeSpawner) {
                                serverCommander->moveUnitsToPosition(singuitiesGoingForDestination, closeSpawner->position());
                            });
                        }
                    }
                });
            }
            catch (std::exception& e)
            {
                Logger::trace("Error while evaluating arcAi: " + std::string(e.what()));
            }
            catch (...)
            {
                Logger::trace("Unkonwn error while evaluating arcAi");
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

        std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>> getNeighbourSpawners(const std::unordered_map<xg::Guid, long long>& strongholds, const std::unordered_map<xg::Guid, std::shared_ptr<Spawner>>& spawnerById)
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
                long long findingSpawnerStronghold = strongholds.at(spawnerFindingNeighbours->id());
                bool isOwnSpawner = findingSpawnerStronghold >= 0;
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

                            if ((strongholds.at(closestSpawnerId) >= 0) != isOwnSpawner)
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

        std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>> reverseMap(const std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>>& neighbourSpawners)
        {
            std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>> reverse;

            for (auto neighbour : neighbourSpawners)
            {
                // creating the empty neighboorhood
                reverse[neighbour.first];
                const auto& neighboureds = neighbour.second;
                for (const auto& neighboured : neighboureds)
                {
                    reverse[neighboured].emplace(neighbour.first);
                }
            }

            return reverse;
        }

        std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>> getAttackersByStrongholds(const std::shared_ptr<const CompleteGameState> completeGameState, const std::shared_ptr<Player>& currentPlayer, const std::unordered_set<xg::Guid> singuitiesWentAttacking)
        {
            std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>> attackersByStrongholds;

            std::shared_ptr<CollisionDetector> allSpawnersCollisionDetectors = getCollisionDetectorForSpawners(completeGameState->spawners());

            for (auto singuity : *currentPlayer->singuities())
            {
                if (singuitiesWentAttacking.count(singuity->id()) > 0)
                    continue;

                allSpawnersCollisionDetectors->getClosest(CollidablePoint(xg::Guid(), singuity->position())).foreach([&attackersByStrongholds, singuity](const xg::Guid& closestSpawnerId) {
                    attackersByStrongholds[closestSpawnerId].emplace(singuity->id());
                });
            }

            return attackersByStrongholds;
        }

        std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>> getNeighbourAttackersByStrongholds(const std::unordered_set<xg::Guid> satellites, const std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>>& neighbours, std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>>& attackersByStronghold, const std::unordered_map<xg::Guid, long long>& strongholds, const std::unordered_map<xg::Guid, std::shared_ptr<Singuity>>& singuityById)
        {
            std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>> neighbourAttackersByStrongholds;

            for (const auto& satelliteId : satellites)
            {
                for (const auto& attackerId : attackersByStronghold[satelliteId])
                {
                    neighbourAttackersByStrongholds[satelliteId].emplace(attackerId);
                }

                std::unordered_set<xg::Guid> freelanceAttackers;
                for (const auto& attackerId : attackersByStronghold[satelliteId])
                {
                    if (strongholds.at(satelliteId) - freelanceAttackers.size() < 0) break;
                    freelanceAttackers.emplace(attackerId);
                }

                for (const auto& seenSpawnerId : neighbours.at(satelliteId))
                {
                    for (const auto& attackerId : freelanceAttackers)
                    {
                        neighbourAttackersByStrongholds[seenSpawnerId].emplace(attackerId);
                    }

                    for (const auto& attackerId : attackersByStronghold[seenSpawnerId])
                    {
                        neighbourAttackersByStrongholds[seenSpawnerId].emplace(attackerId);
                    }
                }
            }

            return neighbourAttackersByStrongholds;
        }

        std::unordered_map<xg::Guid, long long> getNeighbourRelativeStrongholds(const std::unordered_map<xg::Guid, long long>& spawnerStrongholds, std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>>& attackersByNeighbourStronghold)
        {
            std::unordered_map<xg::Guid, long long> neighbourRelativeStrongholds;

            for (const auto& idAndSpawnerStronghold : spawnerStrongholds)
            {
                neighbourRelativeStrongholds[idAndSpawnerStronghold.first] = idAndSpawnerStronghold.second;
                neighbourRelativeStrongholds[idAndSpawnerStronghold.first] += attackersByNeighbourStronghold[idAndSpawnerStronghold.first].size();
            }

            return neighbourRelativeStrongholds;
        }

        std::unordered_map<xg::Guid, double> getNeighbourRelativeDistanceToSpawners(std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>>& attackersByNeighbourStronghold, const std::unordered_map<xg::Guid, std::shared_ptr<Spawner>>& spawnerById, const std::unordered_map<xg::Guid, std::shared_ptr<Singuity>>& singuityById)
        {
            std::unordered_map<xg::Guid, double> neighbourRelativeDistances;

            for (const auto& spawnerIdAndPotentialAttackersId : attackersByNeighbourStronghold)
            {
                const auto spawner = spawnerById.at(spawnerIdAndPotentialAttackersId.first);
                long long attackerCount = 0;
                for (const auto& attackerId : spawnerIdAndPotentialAttackersId.second)
                {
                    neighbourRelativeDistances[spawnerIdAndPotentialAttackersId.first] += spawner->position().distanceSq(singuityById.at(attackerId)->position());
                    ++attackerCount;
                }

                if (attackerCount > 0)
                {
                    neighbourRelativeDistances[spawnerIdAndPotentialAttackersId.first] /= attackerCount;
                }
            }

            return neighbourRelativeDistances;
        }

        Option<xg::Guid> getEasiestTakableStronghold(const std::unordered_map<xg::Guid, long long>& spawnerStrongholds, std::unordered_map<xg::Guid, long long>& spawnerStrongholdsNeighbourRelative, std::unordered_map<xg::Guid, double>& spawnerStrongholdsDistanceToAverageAttackers, std::unordered_map<xg::Guid, std::unordered_set<xg::Guid>>& neighbourAttackersByStronghold, const std::unordered_map<xg::Guid, std::shared_ptr<Spawner>>& spawnerById, const xg::Guid& currentPlayerId)
        {
            Option<xg::Guid> easiestTakableStronghold;

            for (const auto& spawnerStronghold : spawnerStrongholds)
            {
                const auto stronghold = spawnerById.at(spawnerStronghold.first);
                if (!stronghold->canBeInteractedWithBy(currentPlayerId)) continue;

                if (spawnerStrongholdsNeighbourRelative[spawnerStronghold.first] < 0) continue;

                if (neighbourAttackersByStronghold[spawnerStronghold.first].empty()) continue;

                easiestTakableStronghold = easiestTakableStronghold
                    .map<xg::Guid>([&spawnerStronghold, &spawnerStrongholdsNeighbourRelative, &spawnerStrongholdsDistanceToAverageAttackers](const xg::Guid& oldEasiestTakableStronghold) {
                        const auto oldEasiestSpawnerStronghold = spawnerStrongholdsNeighbourRelative[oldEasiestTakableStronghold];
                        const auto checkingEasiestSpawnerStronghold = spawnerStrongholdsNeighbourRelative[spawnerStronghold.first];
                        if (checkingEasiestSpawnerStronghold > oldEasiestSpawnerStronghold)
                        {
                            return spawnerStronghold.first;
                        }
                        else if (checkingEasiestSpawnerStronghold == oldEasiestSpawnerStronghold)
                        {
                            const auto oldEasiestSpawnerDistance = spawnerStrongholdsDistanceToAverageAttackers[oldEasiestTakableStronghold];
                            const auto checkingEasiestSpawnerDistance = spawnerStrongholdsDistanceToAverageAttackers[spawnerStronghold.first];
                            if (checkingEasiestSpawnerDistance < oldEasiestSpawnerDistance)
                            {
                                return spawnerStronghold.first;
                            }
                            else
                            {
                                return oldEasiestTakableStronghold;
                            }
                        }
                        else
                        {
                            return oldEasiestTakableStronghold;
                        }
                    }).getOrElse(spawnerStronghold.first);
            }

            return easiestTakableStronghold;
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
                        if (strongholds.at(neighbourId) >= 0)
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
                const double hundreadthOfSpawnerMaxHealth = spawner->maximumHealthPoint() / 100.0;
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
    };
}