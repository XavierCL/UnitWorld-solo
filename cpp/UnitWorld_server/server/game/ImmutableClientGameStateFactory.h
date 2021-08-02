#pragma once

#include "shared/game/play/CompleteGameState.h"

#include "shared/game/physics/collisions/CollisionDetectorFactory.h"

#include "commons/CollectionPipe.h"

namespace uw
{
    class ImmutableClientGameStateFactory
    {
    public:
        ImmutableClientGameStateFactory(const std::shared_ptr<const CompleteGameState>& completeGameState, const bool& isFogOfWarEnabled, const std::shared_ptr<CollisionDetectorFactory> collisionDetectorFactory):
            _completeGameState(completeGameState),
            _isFogOfWarEnabled(isFogOfWarEnabled),
            _collisionDetectorFactory(collisionDetectorFactory)
        {}

        std::shared_ptr<const CompleteGameState> createGameStateFor(const xg::Guid& playerId) const
        {
            if (!_isFogOfWarEnabled)
            {
                return _completeGameState;
            }
            else
            {
                const Option<std::shared_ptr<Player>> currentPlayerOpt(&_completeGameState->players() | first<std::shared_ptr<Player>>([&playerId](const std::shared_ptr<Player> player){
                    return player->id() == playerId;
                }));

                return currentPlayerOpt
                    .map<std::shared_ptr<const CompleteGameState>>([this, &playerId](const std::shared_ptr<Player> currentPlayer){

                        const std::unordered_map<xg::Guid, std::shared_ptr<Spawner>> spawnersById(*(
                            &_completeGameState->spawners()
                            | toUnorderedMap<xg::Guid, std::shared_ptr<Spawner>>([](const std::shared_ptr<Spawner>& spawner) {
                                return spawner->id();
                            })
                        ));

                        const std::unordered_map<xg::Guid, std::shared_ptr<Singuity>> singuitiesById(*(
                            currentPlayer->singuities()
                            | toUnorderedMap<xg::Guid, std::shared_ptr<Singuity>>([](const std::shared_ptr<Singuity>& singuity) { return singuity->id(); })
                        ));

                        const std::shared_ptr<CollisionDetector> ownSpawnerCollisionDetector(_collisionDetectorFactory->create());

                        ownSpawnerCollisionDetector->updateAllCollidablePoints(
                            *(&_completeGameState->spawners()
                                | filter<std::shared_ptr<Spawner>>([&playerId](const std::shared_ptr<Spawner>& spawner) { return spawner->isClaimedBy(playerId); })
                                | map<CollidablePoint>([](const std::shared_ptr<Spawner>& spawner) { return CollidablePoint(spawner->id(), spawner->position()); })
                                | toVector<CollidablePoint>())
                        );

                        const std::shared_ptr<CollisionDetector> ownSinguitiesCollisionDetector(_collisionDetectorFactory->create());

                        ownSinguitiesCollisionDetector->updateAllCollidablePoints(
                            *(currentPlayer->singuities()
                              | map<CollidablePoint>([](const std::shared_ptr<Singuity> singuity) { return CollidablePoint(singuity->id(), singuity->position()); })
                              | toVector<CollidablePoint>())
                        );

                        return std::make_shared<const CompleteGameState>(
                            getNeutralizedFarSpawners(_completeGameState->spawners(), spawnersById, singuitiesById, ownSpawnerCollisionDetector, ownSinguitiesCollisionDetector, playerId),
                            std::move(*(&_completeGameState->players()
                              | map<std::shared_ptr<Player>>([this, &playerId, &spawnersById, &singuitiesById, &ownSpawnerCollisionDetector, &ownSinguitiesCollisionDetector](const std::shared_ptr<Player> player){
                                  if (player->id() == playerId)
                                  {
                                      return player;
                                  }
                                  else
                                  {
                                      return std::make_shared<Player>(
                                          player->id(),
                                          getCloseEnoughUnits(*player->singuities(), spawnersById, singuitiesById, ownSpawnerCollisionDetector, ownSinguitiesCollisionDetector)
                                      );
                                  }
                              }) | toVector<std::shared_ptr<Player>>())),
                            _completeGameState->frameCount()
                        );
                    }).getOrElse(_completeGameState);
            }
        }

    private:

        static const double SPAWNER_VISION_RADIUS;
        static const double SINGUITY_VISION_RADIUS;

        std::vector<std::shared_ptr<Singuity>> getCloseEnoughUnits(const std::vector<std::shared_ptr<Singuity>>& sourceCollection, const std::unordered_map<xg::Guid, std::shared_ptr<Spawner>>& spawnersById, const std::unordered_map<xg::Guid, std::shared_ptr<Singuity>>& singuitiesById, const std::shared_ptr<CollisionDetector> spawnerCollisionDetector, const std::shared_ptr<CollisionDetector> singuitiesCollisionDetector) const
        {
            return *(&sourceCollection
                | filter<std::shared_ptr<Singuity>>([&spawnersById, &singuitiesById, &spawnerCollisionDetector, &singuitiesCollisionDetector](const std::shared_ptr<Singuity>& filtered) {
                    const auto closestSpawner(spawnerCollisionDetector->getClosest(CollidablePoint(xg::Guid(), filtered->position())));

                    if (closestSpawner.isDefined() && Circle(spawnersById.at(closestSpawner.getOrThrow())->position(), SPAWNER_VISION_RADIUS).contains(filtered->position()))
                    {
                        return true;
                    }

                    const auto closestSinguity(singuitiesCollisionDetector->getClosest(CollidablePoint(xg::Guid(), filtered->position())));

                    if (closestSinguity.isDefined() && Circle(singuitiesById.at(closestSinguity.getOrThrow())->position(), SINGUITY_VISION_RADIUS).contains(filtered->position()))
                    {
                        return true;
                    }

                    return false;
                }) | toVector<std::shared_ptr<Singuity>>());
        }

        std::vector<std::shared_ptr<Spawner>> getNeutralizedFarSpawners(const std::vector<std::shared_ptr<Spawner>>& sourceCollection, const std::unordered_map<xg::Guid, std::shared_ptr<Spawner>>& spawnersById, const std::unordered_map<xg::Guid, std::shared_ptr<Singuity>>& singuitiesById, const std::shared_ptr<CollisionDetector> spawnerCollisionDetector, const std::shared_ptr<CollisionDetector> singuitiesCollisionDetector, const xg::Guid& playerId) const
        {
            return *(&sourceCollection
                | map<std::shared_ptr<Spawner>>([&spawnersById, &singuitiesById, &spawnerCollisionDetector, &singuitiesCollisionDetector, &playerId](const std::shared_ptr<Spawner>& filtered) {

                    if (filtered->isClaimedBy(playerId)) return filtered;

                    const auto closestSpawner(spawnerCollisionDetector->getClosest(CollidablePoint(xg::Guid(), filtered->position())));

                    if (closestSpawner.isDefined() && Circle(spawnersById.at(closestSpawner.getOrThrow())->position(), SPAWNER_VISION_RADIUS).contains(filtered->position()))
                    {
                        return filtered;
                    }

                    const auto closestSinguity(singuitiesCollisionDetector->getClosest(CollidablePoint(xg::Guid(), filtered->position())));

                    if (closestSinguity.isDefined() && Circle(singuitiesById.at(closestSinguity.getOrThrow())->position(), SINGUITY_VISION_RADIUS).contains(filtered->position()))
                    {
                        return filtered;
                    }

                    return std::make_shared<Spawner>(filtered->id(), filtered->position(), Options::None<SpawnerAllegence>(), Options::None<MobileUnitDestination>(), 0, 0, 0);
                }) | toVector<std::shared_ptr<Spawner>>());
        }

        const std::shared_ptr<const CompleteGameState> _completeGameState;
        const bool _isFogOfWarEnabled;
        const std::shared_ptr<CollisionDetectorFactory> _collisionDetectorFactory;
    };
}