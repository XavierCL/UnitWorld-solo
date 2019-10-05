#pragma once

#include "shared/game/play/units/Singuity.h"
#include "shared/game/play/units/UnitWithHealthPoint.h"

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

        void shootEnemy(std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<UnitWithHealthPoint>>> shootablesById, const unsigned long long& frameTimestamp)
        {
            auto closestEnemy = _closestEnemyId.flatMap<std::shared_ptr<UnitWithHealthPoint>>([shootablesById](const xg::Guid& closestEnemyId) {
                return shootablesById | find<std::shared_ptr<UnitWithHealthPoint>>(closestEnemyId);
            });

            closestEnemy.foreach([this, &frameTimestamp](std::shared_ptr<UnitWithHealthPoint> enemy) {
                _singuity->shootIfCan(enemy, frameTimestamp);
            });
        }

        void actualize(const std::unordered_map<xg::Guid, std::shared_ptr<Spawner>>& spawnersById, std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<UnitWithHealthPoint>>> shootablesById)
        {
            auto closestThing = _closestThingId.flatMap<std::shared_ptr<UnitWithHealthPoint>>([shootablesById](const xg::Guid& closestThingId) {
                return shootablesById | find<std::shared_ptr<UnitWithHealthPoint>>(closestThingId);
            });

            Vector2D repulsionForce;
            closestThing.foreach([this, &repulsionForce](std::shared_ptr<UnitWithHealthPoint> thing) {

                const double minDistance = 4;
                const double maxDistance = 30;
                const double maxRepulsionForce = _singuity->maximumAcceleration() * 0.2;

                Vector2D repulsionDistance(_singuity->position() - thing->position());

                if (repulsionDistance.moduleSq() <= minDistance * minDistance)
                {
                    repulsionForce = repulsionDistance.atModule(maxRepulsionForce);
                }
                else if (repulsionDistance.moduleSq() >= maxDistance * maxDistance)
                {
                    repulsionForce = Vector2D(0.0, 0.0);
                }
                else
                {
                    const double a = -1.0 / (maxDistance - minDistance);
                    const double b = maxDistance / (maxDistance - minDistance);
                    repulsionForce = repulsionDistance.atModule(maxRepulsionForce * (repulsionDistance.module() * a + b));
                }

            });

            Vector2D acceleration = _singuity->destination().flatMap<Vector2D>([this, &spawnersById](const std::variant<Vector2D, SpawnerDestination>& destination) {
                return std::visit(overloaded{
                    [this](const Vector2D& point) {
                        bool willBreakForDestination = (_singuity->position() + _singuity->speed().atModule(_singuity->stopDistanceFromTargetSq())).distanceSq(point) < 100;

                        if (willBreakForDestination)
                        {
                            bool isReleaseBreakSpeed = _singuity->speed().moduleSq() < 0.01 * _singuity->maximumSpeed() * _singuity->maximumSpeed();

                            if (isReleaseBreakSpeed)
                            {
                                _singuity->clearDestination();
                                return Options::None<Vector2D>();
                            }
                            else
                            {
                                return Options::Some(_singuity->getBreakingAcceleration());
                            }
                        }
                        else
                        {
                            return Options::Some(_singuity->getMaximalAcceleration(point));
                        }
                    }, [this, &spawnersById](const SpawnerDestination spawnerDestination) {
                        return (&spawnersById | find<std::shared_ptr<Spawner>>(spawnerDestination.spawnerId())).map<Vector2D>([this, &spawnerDestination](std::shared_ptr<Spawner> spawner) {
                            if (spawner->hasSameAllegenceState(spawnerDestination.spawnerAllegence(), _playerId))
                            {
                                if (spawner->canBeReguvenatedBy(_playerId) && Circle(spawner->position(), 8).contains(_singuity->position()))
                                {
                                    spawner->reguvenate(_playerId, _singuity);
                                    return Vector2D();
                                }
                                else if (spawner->canBeAttackedBy(_playerId) && Circle(spawner->position(), 8).contains(_singuity->position()))
                                {
                                    spawner->attackedBy(_playerId, _singuity);
                                    return Vector2D();
                                }
                                else
                                {
                                    return _singuity->getMaximalAcceleration(spawner->position());
                                }
                            }
                            else
                            {
                                _singuity->setPointDestination(spawner->position());
                                return _singuity->getMaximalAcceleration(spawner->position());
                            }
                        });
                    }
                }, destination);
            }).getOrElse([this]() {
                return _singuity->getSlowBreakingAcceleration();
            });

            acceleration = acceleration.maxAt(_singuity->maximumAcceleration());
            acceleration += repulsionForce;

            _singuity->actualizeAcceleration(acceleration);
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

        const std::shared_ptr<Singuity> _singuity;
        const xg::Guid _playerId;
        Option<xg::Guid> _closestEnemyId;
        Option<xg::Guid> _closestThingId;
    };
}