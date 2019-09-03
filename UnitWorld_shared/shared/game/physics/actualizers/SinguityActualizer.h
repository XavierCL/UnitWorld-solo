#pragma once

#include "shared/game/play/Singuity.h"
#include "shared/game/play/UnitWithHealthPoint.h"

#include "shared/game/physics/collisions/CollisionDetector.h"

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

        void updateShootingAndRepulsionForce(const xg::Guid& playerId, std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<CollisionDetector>>> collisionDetectorsByPlayerId, std::shared_ptr<CollisionDetector> neutralCollisionDetector, std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<UnitWithHealthPoint>>> shootablesById, const unsigned long long& frameTimestamp)
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

        void actualize()
        {
            Vector2D acceleration = _singuity->destination().flatMap<Vector2D>([this](const Vector2D& destination) {
                _singuity->setIsBreakingForDestination(_singuity->position().distanceSq(destination) < _singuity->stopDistanceFromTargetSq() || _singuity->isBreakingForDestination());

                if (_singuity->isBreakingForDestination())
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
                    return Options::Some(_singuity->getMaximalAcceleration(destination));
                }
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
        const std::shared_ptr<Singuity> _singuity;

        Vector2D _repulsionForce;
    };
}