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
        SinguityActualizer(std::shared_ptr<Singuity> singuity):
            _singuity(singuity)
        {}

        void updateShootingAndRepulsionForce(const xg::Guid& playerId, std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<CollisionDetector>>> collisionDetectorsByPlayerId, std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<UnitWithHealthPoint>>> shootablesById, const unsigned long long& frameTimestamp)
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

            closestEnemy.foreach([this, &frameTimestamp](std::shared_ptr<UnitWithHealthPoint> enemy) {
                _singuity->shootIfCan(enemy, frameTimestamp);
            });

            closestThing.foreach([this](std::shared_ptr<UnitWithHealthPoint> thing) {
                _repulsionForce = (_singuity->position() - thing->position()).divide(0.005, 10.0);
            });
        }

        void actualize(const xg::Guid& playerId, const std::unordered_map<xg::Guid, std::shared_ptr<Spawner>>& spawnersById)
        {
            Vector2D acceleration = _singuity->destination().flatMap<Vector2D>([this, &playerId, &spawnersById](const std::variant<Vector2D, SpawnerDestination>& destination) {
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
                    }, [this, &playerId, &spawnersById](const SpawnerDestination spawnerDestination) {
                        return (&spawnersById | find<std::shared_ptr<Spawner>>(spawnerDestination.spawnerId())).map<Vector2D>([this, &playerId, &spawnerDestination](std::shared_ptr<Spawner> spawner) {
                            if (spawner->hasSameAllegenceState(spawnerDestination.spawnerAllegence()))
                            {
                                if (spawner->canBeReguvenatedBy(playerId) && Circle(spawner->position(), 8).contains(_singuity->position()))
                                {
                                    spawner->reguvenate(playerId, _singuity);
                                    return Vector2D();
                                }
                                else if (spawner->canBeAttackedBy(playerId) && Circle(spawner->position(), 8).contains(_singuity->position()))
                                {
                                    spawner->attackedBy(playerId, _singuity);
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

            acceleration += _repulsionForce.maxAt(_singuity->maximumAcceleration());
            acceleration = acceleration.maxAt(_singuity->maximumAcceleration());

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

        Vector2D _repulsionForce;
    };
}