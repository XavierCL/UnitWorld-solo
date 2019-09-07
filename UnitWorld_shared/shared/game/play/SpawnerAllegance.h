#pragma once

#include "commons/Guid.hpp"

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

        bool hasSameState(const SpawnerAllegence& allegence) const
        {
            return _isClaimed == allegence._isClaimed && _allegedPlayerId == allegence._allegedPlayerId;
        }

    private:
        const bool _isClaimed;
        const double _healthPoint;
        const xg::Guid _allegedPlayerId;
    };
}