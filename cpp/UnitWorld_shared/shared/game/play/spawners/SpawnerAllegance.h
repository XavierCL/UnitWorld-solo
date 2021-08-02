#pragma once

#include "commons/Guid.hpp"
#include "commons/Option.hpp"

namespace uw
{
    class SpawnerAllegence
    {
    public:

        SpawnerAllegence(const bool& isClaimed, const double& healthPoint, const xg::Guid& playerId) :
            _isClaimed(isClaimed),
            _healthPoint(healthPoint),
            _allegedPlayerId(playerId)
        {}

        bool isClaimed() const
        {
            return _isClaimed;
        }

        double healthPoint() const
        {
            return _healthPoint;
        }

        xg::Guid allegedPlayerId() const
        {
            return _allegedPlayerId;
        }

        SpawnerAllegence loseHealthPoint(const double& healthPoint) const
        {
            return SpawnerAllegence(_isClaimed, _healthPoint - healthPoint, _allegedPlayerId);
        }

        SpawnerAllegence gainHealthPoint(const double& healthPoint) const
        {
            return SpawnerAllegence(_isClaimed, _healthPoint + healthPoint, _allegedPlayerId);
        }

        bool hasSameState(const SpawnerAllegence& allegence, const xg::Guid& currentPlayerId, const double& maxHealthPoint) const
        {
            if (currentPlayerId == _allegedPlayerId || allegence._allegedPlayerId == currentPlayerId)
            {
                return _isClaimed == allegence._isClaimed
                    && (_healthPoint == maxHealthPoint) == (allegence._healthPoint == maxHealthPoint);
            }
            else
            {
                return _isClaimed == allegence._isClaimed;
            }
        }

    private:
        const bool _isClaimed;
        const double _healthPoint;
        const xg::Guid _allegedPlayerId;
    };
}