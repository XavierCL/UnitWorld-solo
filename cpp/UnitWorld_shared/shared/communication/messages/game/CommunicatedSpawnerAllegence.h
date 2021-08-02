#pragma once

#include "shared/communication/messages/game/CommunicatedVector2D.h"

#include "commons/Guid.hpp"
#include "commons/Option.hpp"

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

        const static std::string IS_CLAIMED_LABEL;
        const static std::string HEALTH_POINTS_LABEL;
        const static std::string PLAYER_ID_LABEL;

        const bool _isClaimed;
        const double _healthPoint;
        const xg::Guid _playerId;
    };
}