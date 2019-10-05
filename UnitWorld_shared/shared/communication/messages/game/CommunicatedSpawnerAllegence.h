#pragma once

#include "commons/Guid.hpp"

#include <nlohmann/json.hpp>

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

        nlohmann::json toJson() const;

        static CommunicatedSpawnerAllegence fromJson(const nlohmann::json& jsonData);

    private:

        const static std::string CommunicatedSpawnerAllegence::IS_CLAIMED_LABEL;
        const static std::string CommunicatedSpawnerAllegence::HEALTH_POINTS_LABEL;
        const static std::string CommunicatedSpawnerAllegence::PLAYER_ID_LABEL;

        const bool _isClaimed;
        const double _healthPoint;
        const xg::Guid _playerId;
    };
}