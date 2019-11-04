#pragma once

#include "SpawnerAllegance.h"

#include "shared/game/play/units/Singuity.h"
#include "shared/game/play/units/UnitWithHealthPoint.h"
#include "shared/game/play/units/MobileUnitDestination.h"

#include "shared/game/geometry/Vector2D.h"

#include "commons/Option.hpp"
#include "commons/Guid.hpp"

namespace uw
{
    class Spawner: virtual public UnitWithHealthPoint
    {
    public:
        Spawner(const xg::Guid& id, const Vector2D& initialPosition, Option<SpawnerAllegence> allegence, const Option<MobileUnitDestination> rally, const unsigned long long& lastSpawnFameCount, const unsigned long long& totalSpawnedCount, const long long& lastClaimedFrameCount) :
            Unit(id, initialPosition),
            UnitWithHealthPoint(id, initialPosition, allegence.map<double>([](const SpawnerAllegence& spawnerAllegence) {
                return spawnerAllegence.healthPoint();
            }).getOrElse(0.0)),
            _allegence(allegence),
            _rally(rally),
            _lastSpawnFrameCount(lastSpawnFameCount),
            _totalSpawnedCount(totalSpawnedCount),
            _lastClaimedFrameCount(lastClaimedFrameCount)
        {}

        Spawner(const Vector2D& initialPosition, const xg::Guid& playerId) :
            Unit(initialPosition),
            UnitWithHealthPoint(initialPosition, maximumHealthPoint()),
            _allegence(Options::Some(SpawnerAllegence(true, maximumHealthPoint(), playerId))),
            _rally(),
            _lastSpawnFrameCount(0),
            _totalSpawnedCount(0),
            _lastClaimedFrameCount(-gestationFrameLag())
        {}

        Spawner(const Vector2D& initialPosition) :
            Unit(initialPosition),
            UnitWithHealthPoint(initialPosition, 0),
            _allegence(Options::None<SpawnerAllegence>()),
            _rally(),
            _lastSpawnFrameCount(0),
            _totalSpawnedCount(0),
            _lastClaimedFrameCount(-gestationFrameLag())
        {}

        Spawner(const Spawner& copy):
            Unit(copy),
            UnitWithHealthPoint(copy.position(), copy._allegence.map<double>([](const SpawnerAllegence& spawnerAllegence) {
                return spawnerAllegence.healthPoint();
            }).getOrElse(0.0)),
            _allegence(copy._allegence),
            _rally(copy._rally),
            _lastSpawnFrameCount(copy._lastSpawnFrameCount),
            _totalSpawnedCount(copy._totalSpawnedCount),
            _lastClaimedFrameCount(copy._lastClaimedFrameCount)
        {}

        Option<SpawnerAllegence> allegence() const
        {
            return _allegence;
        }

        Option<MobileUnitDestination> rally() const
        {
            return _rally;
        }

        void spawnIfCan(const std::function<void(xg::Guid, std::shared_ptr<Singuity>)>& spawned, const double& frameCount)
        {
            _allegence.foreach([this, &spawned, &frameCount](const SpawnerAllegence& allegence) {
                if (allegence.isClaimed() && _lastClaimedFrameCount + gestationFrameLag() <= frameCount && _lastSpawnFrameCount + spawnFrameLag() <= frameCount)
                {
                    _lastSpawnFrameCount = frameCount;
                    ++_totalSpawnedCount;

                    const std::shared_ptr<Singuity> spawnedSinguity = std::make_shared<Singuity>(Singuity::spawn(
                        position(),
                        UNIT_SPAWN_DIRECTION[_totalSpawnedCount % UNIT_SPAWN_DIRECTION.size()],
                        _rally
                    ));

                    spawned(allegence.allegedPlayerId(), spawnedSinguity);
                }
            });
        }

        bool isAllegedToPlayer(const xg::Guid& playerId) const
        {
            return _allegence
                .map<bool>([&playerId](const SpawnerAllegence& allegence) {return allegence.allegedPlayerId() == playerId; })
                .getOrElse(false);
        }

        bool isClaimedBy(const xg::Guid& playerId) const
        {
            return _allegence
                .map<bool>([&playerId](const SpawnerAllegence& allegence) {return allegence.isClaimed() && allegence.allegedPlayerId() == playerId; })
                .getOrElse(false);
        }

        bool isClaimed() const
        {
            return _allegence.map<bool>([](const SpawnerAllegence& allegence) { return allegence.isClaimed(); }).getOrElse(false);
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
                if (isDead())
                {
                    _rally = Options::None<MobileUnitDestination>();
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

        bool hasSameAllegenceState(const Option<SpawnerAllegence>& otherAllegence, const xg::Guid& playerId)
        {
            return otherAllegence.map<bool>([this, &playerId](const SpawnerAllegence& allegence) {
                return _allegence.map<bool>([this, &allegence, &playerId](const SpawnerAllegence& ownAllegence) {
                    return allegence.hasSameState(ownAllegence, playerId, maximumHealthPoint());
                }).getOrElse([&allegence]() { return !allegence.isClaimed(); });
            }).getOrElse([this]() {
                return _allegence
                    .map<bool>([](const SpawnerAllegence& allegence) { return !allegence.isClaimed(); })
                    .getOrElse(true);
            });
        }

        bool canBeReguvenatedBy(const xg::Guid& playerId) const
        {
            return isAllegedToPlayer(playerId) && !isAtMaximumHealth() && !isClaimed() || _allegence.isEmpty();
        }

        bool canBeAttackedBy(const xg::Guid& playerId) const
        {
            return !isAllegedToPlayer(playerId) && healthPoint() > 0 && _allegence.isDefined();
        }

        bool canBeInteractedWithBy(const xg::Guid& playerId) const
        {
            return canBeReguvenatedBy(playerId) || canBeAttackedBy(playerId);
        }

        void reguvenate(const xg::Guid& playerId, std::shared_ptr<Singuity> reguvenator)
        {
            if (canBeReguvenatedBy(playerId))
            {
                double gainedHealthPoint = healthPoint() + reguvenator->reguvenatingHealth() > maximumHealthPoint()
                    ? maximumHealthPoint() - healthPoint()
                    : reguvenator->reguvenatingHealth();
                UnitWithHealthPoint::gainHealthPoint(gainedHealthPoint);
                _allegence = _allegence.map<SpawnerAllegence>([&gainedHealthPoint](const SpawnerAllegence& oldAllegence) {
                    return oldAllegence.gainHealthPoint(gainedHealthPoint);
                }).orElse([&playerId, &gainedHealthPoint]() {
                    return SpawnerAllegence(false, gainedHealthPoint, playerId);
                });
                reguvenator->makeHealthPointNone();
            }
        }

        void setRally(const MobileUnitDestination& rally)
        {
            _rally = Options::Some(rally);
        }

        double maximumHealthPoint() const override
        {
            return 5000.0;
        }

        unsigned long long lastSpawnFrameCount() const
        {
            return _lastSpawnFrameCount;
        }

        unsigned long long totalSpawnedCount() const
        {
            return _totalSpawnedCount;
        }

    private:
        static std::vector<Vector2D> UNIT_SPAWN_DIRECTION;

        static unsigned long long spawnFrameLag()
        {
            return 30;
        }

        static unsigned long long gestationFrameLag()
        {
            return 30000;
        }

        Option<SpawnerAllegence> _allegence;
        Option<MobileUnitDestination> _rally;
        unsigned long long _lastSpawnFrameCount;
        unsigned long long _totalSpawnedCount;
        unsigned long long _lastClaimedFrameCount;
    };
}