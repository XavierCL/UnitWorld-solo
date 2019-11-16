#pragma once

#include "commons/Guid.hpp"

namespace uw
{
    class CommunicatedSpawnerAllegence
    {
    public:
        CommunicatedSpawnerAllegence(const bool& isClaimed, const double& healthPoint, const xg::Guid& playerId) :
            _isClaimed(isClaimed),
            _healthPoint(healthPoint),
            _playerId(playerId)
        {}

        bool isClaimed() const
        {
            return _isClaimed;
        }

        double healthPoint() const
        {
            return _healthPoint;
        }

        xg::Guid playerId() const
        {
            return _playerId;
        }

        std::string toJson() const;

        static CommunicatedSpawnerAllegence fromJson(const std::string& jsonData);

    private:
        const bool _isClaimed;
        const double _healthPoint;
        const xg::Guid _playerId;
    };
}