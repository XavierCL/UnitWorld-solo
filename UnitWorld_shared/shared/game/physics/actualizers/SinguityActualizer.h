#pragma once

#include "shared/game/play/units/Singuity.h"
#include "shared/game/play/units/UnitWithHealthPoint.h"
#include "shared/game/play/spawners/Spawner.h"

#include "shared/game/physics/collisions/CollisionDetector.h"
#include "shared/game/geometry/Circle.h"

#include "commons/Option.hpp"
#include "commons/CollectionPipe.h"

namespace uw
{
    class SinguityActualizer
    {
    public:
        SinguityActualizer(std::shared_ptr<Singuity> singuity, const xg::Guid& playerId):
            _singuity(singuity),
            _playerId(playerId)
        {}

        void updateCollisions(const xg::Guid& playerId, std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<CollisionDetector>>> collisionDetectorsByPlayerId, std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<UnitWithHealthPoint>>> shootablesById)
        {
            Option<std::shared_ptr<UnitWithHealthPoint>> closestThing;
            Option<std::shared_ptr<UnitWithHealthPoint>> closestEnemy;
            for (const auto& playerIdAndCollisionDetector : *collisionDetectorsByPlayerId)
            {
                const auto& collisionDetectorPlayerId = playerIdAndCollisionDetector.first;
                const auto& collisionDetector = playerIdAndCollisionDetector.second;

                auto closestCollidingId = collisionDetector->getClosest(CollidablePoint(_singuity->id(), _singuity->position()));

                closestCollidingId.foreach([shootablesById, this, &playerId, &collisionDetectorPlayerId, &closestThing, &closestEnemy](const xg::Guid& closestShootableId) {
                    auto closestShootable = shootablesById | find<std::shared_ptr<UnitWithHealthPoint>>(closestShootableId);
                    closestShootable.foreach([this, &playerId, &collisionDetectorPlayerId, &closestThing, &closestEnemy](std::shared_ptr<UnitWithHealthPoint> unitWithHealthPoint) {

                        closestThing = closestThing.map<std::shared_ptr<UnitWithHealthPoint>>([this, unitWithHealthPoint](std::shared_ptr<UnitWithHealthPoint> thing) {
                            if (unitWithHealthPoint->position().distanceSq(_singuity->position()) < thing->position().distanceSq(_singuity->position()))
                            {
                                return unitWithHealthPoint;
                            }
                            else
                            {
                                return thing;
                            }
                        }).orElse(Options::Some(unitWithHealthPoint));

                        if (playerId != collisionDetectorPlayerId)
                        {
                            closestEnemy = closestEnemy.map<std::shared_ptr<UnitWithHealthPoint>>([this, &playerId, &collisionDetectorPlayerId, unitWithHealthPoint](std::shared_ptr<UnitWithHealthPoint> enemy) {
                                if (unitWithHealthPoint->position().distanceSq(_singuity->position()) < enemy->position().distanceSq(_singuity->position()))
                                {
                                    return unitWithHealthPoint;
                                }
                                else
                                {
                                    return enemy;
                                }
                            }).orElse(Options::Some(unitWithHealthPoint));
                        }
                    });
                });
            }

            _closestEnemyId = closestEnemy.map<xg::Guid>([](std::shared_ptr<UnitWithHealthPoint> enemy) {
                return enemy->id();
            });

            _closestThingId = closestThing.map<xg::Guid>([](std::shared_ptr<UnitWithHealthPoint> thing) {
                return thing->id();
            });
        }

        void shootEnemy(std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<UnitWithHealthPoint>>> shootablesById, const unsigned long long& frameCount)
        {
            auto closestEnemy = _closestEnemyId.flatMap<std::shared_ptr<UnitWithHealthPoint>>([shootablesById](const xg::Guid& closestEnemyId) {
                return shootablesById | findSafe<std::shared_ptr<UnitWithHealthPoint>>(closestEnemyId);
            });

            closestEnemy.foreach([this, &frameCount](std::shared_ptr<UnitWithHealthPoint> enemy) {
                _singuity->shootIfCan(enemy, frameCount);
            });
        }

        void actualize(const std::unordered_map<xg::Guid, std::shared_ptr<Spawner>>& spawnersById, std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<UnitWithHealthPoint>>> shootablesById)
        {
            Vector2D acceleration = _singuity->destination().flatMap<Vector2D>([this, &spawnersById](const MobileUnitDestination& destination) {
                return destination.map<Option<Vector2D>>(
                    [this](const Vector2D& point) {
                        return getOptimalAccelerationTowards(point).orExecute([this]() { _singuity->clearDestination(); });
                    }, [this, &spawnersById](const SpawnerDestination spawnerDestination) {
                        return (&spawnersById | find<std::shared_ptr<Spawner>>(spawnerDestination.spawnerId())).flatMap<Vector2D>([this, &spawnerDestination](std::shared_ptr<Spawner> spawner) {
                            if (spawner->hasSameAllegenceState(spawnerDestination.spawnerAllegence(), _playerId))
                            {
                                if (spawner->canBeReguvenatedBy(_playerId) && Circle(spawner->position(), ACCEPTABLE_DISTANCE_TO_SPAWNER_DESTINATION).contains(_singuity->position()))
                                {
                                    spawner->reguvenate(_playerId, _singuity);
                                    return Options::None<Vector2D>();
                                }
                                else if (spawner->canBeAttackedBy(_playerId) && Circle(spawner->position(), ACCEPTABLE_DISTANCE_TO_SPAWNER_DESTINATION).contains(_singuity->position()))
                                {
                                    spawner->attackedBy(_playerId, _singuity);
                                    return Options::None<Vector2D>();
                                }
                                else
                                {
                                    return getOptimalAccelerationTowards(spawner->position());
                                }
                            }
                            else
                            {
                                _singuity->setPointDestination(spawner->position());
                                return getOptimalAccelerationTowards(spawner->position());
                            }
                        });
                    },
                    [this, &spawnersById](const xg::Guid& unconditionalSpawnerDestination) {
                        return (&spawnersById | find<std::shared_ptr<Spawner>>(unconditionalSpawnerDestination)).map<Vector2D>([this](std::shared_ptr<Spawner> spawner) {
                            if (Circle(spawner->position(), ACCEPTABLE_DISTANCE_TO_SPAWNER_DESTINATION).contains(_singuity->position()))
                            {
                                if (spawner->canBeReguvenatedBy(_playerId))
                                {
                                    spawner->reguvenate(_playerId, _singuity);
                                }
                                else if (spawner->canBeAttackedBy(_playerId))
                                {
                                    spawner->attackedBy(_playerId, _singuity);
                                }
                                else
                                {
                                    _singuity->clearDestination();
                                }
                                return Options::None<Vector2D>();
                            }
                            else
                            {
                                return getOptimalAccelerationTowards(spawner->position());
                            }
                        });
                    }
                );
            }).getOrElse([this]() {
                return _singuity->getSlowBreakingAcceleration();
            });

            acceleration = acceleration.maxAt(_singuity->maximumAcceleration());

            _singuity->actualizeSpeed(acceleration);

            auto closestThing = _closestThingId.flatMap<std::shared_ptr<UnitWithHealthPoint>>([shootablesById](const xg::Guid& closestThingId) {
                return shootablesById | findSafe<std::shared_ptr<UnitWithHealthPoint>>(closestThingId);
            });

            Vector2D repulsionSpeed;
            closestThing.foreach([this, &repulsionSpeed](std::shared_ptr<UnitWithHealthPoint> thing) {
                Vector2D repulsionDistance(_singuity->position() - thing->position());

                const double maxRepulsionSpeed = _singuity->maximumSpeed() * REPULSION_MAX_SPEED_RATIO;

                if (repulsionDistance.moduleSq() <= REPULSION_MIN_VARIABLE_DISTANCE * REPULSION_MIN_VARIABLE_DISTANCE)
                {
                    repulsionSpeed = repulsionDistance.atModule(maxRepulsionSpeed);
                }
                else if (repulsionDistance.moduleSq() >= REPULSION_MAX_VARIABLE_DISTANCE * REPULSION_MAX_VARIABLE_DISTANCE)
                {
                    repulsionSpeed = Vector2D(0.0, 0.0);
                }
                else
                {
                    const double a = -1.0 / (REPULSION_MAX_VARIABLE_DISTANCE - REPULSION_MIN_VARIABLE_DISTANCE);
                    const double b = REPULSION_MAX_VARIABLE_DISTANCE / (REPULSION_MAX_VARIABLE_DISTANCE - REPULSION_MIN_VARIABLE_DISTANCE);
                    repulsionSpeed = repulsionDistance.atModule(maxRepulsionSpeed * (repulsionDistance.module() * a + b));
                }
            });

            _singuity->actualizePosition(repulsionSpeed);
        }

        std::shared_ptr<Singuity> singuity()
        {
            return _singuity;
        }

    private:
        static double sq(const double& value)
        {
            return value * value;
        }

        Option<Vector2D> getOptimalAccelerationTowards(const Vector2D& position)
        {
            bool willBreakForDestination = (_singuity->position() + _singuity->speed().atModule(_singuity->stopDistanceFromTargetSq())).distanceSq(position) < ACCEPTABLE_DISTANCE_TO_POSITION_DESTINATION * ACCEPTABLE_DISTANCE_TO_POSITION_DESTINATION;

            if (willBreakForDestination)
            {
                bool isReleaseBreakSpeed = _singuity->speed().moduleSq() < ACCEPTABLE_SPEED_RATIO_FOR_STOP_BREAKING * _singuity->maximumSpeed() * _singuity->maximumSpeed();

                if (isReleaseBreakSpeed)
                {
                    return Options::None<Vector2D>();
                }
                else
                {
                    return Options::Some(_singuity->getBreakingAcceleration());
                }
            }
            else
            {
                return Options::Some(_singuity->getMaximalAcceleration(position));
            }
        }

        const static double REPULSION_MIN_VARIABLE_DISTANCE;
        const static double REPULSION_MAX_VARIABLE_DISTANCE;
        const static double REPULSION_MAX_SPEED_RATIO;
        const static double ACCEPTABLE_DISTANCE_TO_POSITION_DESTINATION;
        const static double ACCEPTABLE_DISTANCE_TO_SPAWNER_DESTINATION;
        const static double ACCEPTABLE_SPEED_RATIO_FOR_STOP_BREAKING;

        const std::shared_ptr<Singuity> _singuity;
        const xg::Guid _playerId;
        Option<xg::Guid> _closestEnemyId;
        Option<xg::Guid> _closestThingId;
    };
}