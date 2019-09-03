#pragma once

#include "Singuity.h"
#include "SpawnerAllegance.h"

#include "UnitWithHealthPoint.h"

#include "shared/game/geometry/Vector2D.h"

#include "commons/Option.hpp"
#include "commons/Guid.hpp"

namespace uw
{
    class Spawner: virtual public UnitWithHealthPoint
    {
    public:
        Spawner(const xg::Guid& id, const Vector2D& initialPosition, Option<SpawnerAllegence> allegence, const unsigned long long& lastSpawnTimestamp, const unsigned long long& totalSpawnedCount) :
            Unit(id, initialPosition),
            UnitWithHealthPoint(id, initialPosition, allegence.map<double>([](const SpawnerAllegence& spawnerAllegence) {
                return spawnerAllegence.healthPoint();
            }).getOrElse(0.0)),
            _allegence(allegence),
            _lastSpawnTimestamp(lastSpawnTimestamp),
            _totalSpawnedCount(totalSpawnedCount)
        {}

        Spawner(const Vector2D& initialPosition, const xg::Guid& playerId) :
            Unit(initialPosition),
            UnitWithHealthPoint(initialPosition, maximumHealthPoint()),
            _allegence(Options::Some(SpawnerAllegence(true, maximumHealthPoint(), playerId))),
            _lastSpawnTimestamp(0),
            _totalSpawnedCount(0)
        {}

        Spawner(const Vector2D& initialPosition) :
            Unit(initialPosition),
            UnitWithHealthPoint(initialPosition, 0),
            _allegence(Options::None<SpawnerAllegence>()),
            _lastSpawnTimestamp(0),
            _totalSpawnedCount(0)
        {}

        Spawner(const Spawner& copy):
            Unit(copy),
            UnitWithHealthPoint(copy.position(), copy._allegence.map<double>([](const SpawnerAllegence& spawnerAllegence) {
            return spawnerAllegence.healthPoint();
        }).getOrElse(0.0)),
            _allegence(copy._allegence),
            _lastSpawnTimestamp(copy._lastSpawnTimestamp),
            _totalSpawnedCount(copy._totalSpawnedCount)
        {}

        Option<SpawnerAllegence> allegence() const
        {
            return _allegence;
        }

        void spawnIfCan(const std::function<void(xg::Guid, std::shared_ptr<Singuity>)>& spawned, const double& frameTimestamp)
        {
            _allegence.foreach([this, &spawned, &frameTimestamp](const SpawnerAllegence& allegence) {
                if (allegence.isClaimed() && _lastSpawnTimestamp + spawnTimeLag() <= frameTimestamp)
                {
                    _lastSpawnTimestamp = frameTimestamp;
                    ++_totalSpawnedCount;
                    spawned(allegence.allegedPlayerId(), std::make_shared<Singuity>(Singuity::spawn(position(), UNIT_SPAWN_DIRECTION[_totalSpawnedCount % UNIT_SPAWN_DIRECTION.size()])));
                }
            });
        }

        virtual void loseHealthPoint(const double& healthPoint) override
        {
            _allegence = _allegence.map<SpawnerAllegence>([this, &healthPoint](const SpawnerAllegence& allegence) {
                const double minZeroHealthPoint = allegence.healthPoint() - healthPoint < 0 ? allegence.healthPoint() : healthPoint;
                UnitWithHealthPoint::loseHealthPoint(minZeroHealthPoint);
                return allegence.loseHealthPoint(minZeroHealthPoint);
            });
        }

        void updateAllegence()
        {
            _allegence = _allegence.flatMap<SpawnerAllegence>([this](const SpawnerAllegence& oldAllegence) {
                if (oldAllegence.isClaimed() && isDead())
                {
                    return Options::None<SpawnerAllegence>();
                }
                else if (!oldAllegence.isClaimed() && oldAllegence.healthPoint() >= maximumHealthPoint())
                {
                    return Options::Some(SpawnerAllegence(true, maximumHealthPoint(), oldAllegence.allegedPlayerId()));
                }
                else
                {
                    return Options::Some(oldAllegence);
                }
            });
        }

        // bool reguvenate(), which only works if player id == allegence player id or there are no allegence.
        // It creates an allegence if none and adds up to the spawner life
        // it does nothing and returns false if the spawner life is already at max

        double maximumHealthPoint() const override
        {
            return 2000.0;
        }

        unsigned long long lastSpawnTimestamp() const
        {
            return _lastSpawnTimestamp;
        }

        unsigned long long totalSpawnedCount() const
        {
            return _totalSpawnedCount;
        }

    private:
        static std::vector<Vector2D> UNIT_SPAWN_DIRECTION;

        static unsigned long long spawnTimeLag()
        {
            return 1000000000;
        }

        Option<SpawnerAllegence> _allegence;
        unsigned long long _lastSpawnTimestamp;
        unsigned long long _totalSpawnedCount;
    };
}