#pragma once

#include "CommunicatedVector2D.h"
#include "CommunicatedSinguityDestination.h"

#include "commons/Guid.hpp"
#include "commons/Option.hpp"

namespace uw
{
    class CommunicatedSinguity
    {
    public:
        CommunicatedSinguity(const xg::Guid& singuityId, const xg::Guid& playerId, const CommunicatedVector2D& position, const CommunicatedVector2D& speed, const Option<CommunicatedSinguityDestination>& destination, const double& healthPoint, const long long& lastShootFrameCount);

        nlohmann::json toJson() const;

        static CommunicatedSinguity fromJson(const nlohmann::json& jsonData);

        xg::Guid singuityId() const;
        xg::Guid playerId() const;

        CommunicatedVector2D position() const;
        CommunicatedVector2D speed() const;
        Option<CommunicatedSinguityDestination> destination() const;
        double healthPoint() const;
        long long lastShootFrameCount() const;

    private:

        const static std::string NO_DESTINATION_VALUE;
        const static std::string SINGUITY_ID_LABEL;
        const static std::string PLAYER_ID_LABEL;
        const static std::string POSITION_LABEL;
        const static std::string SPEED_LABEL;
        const static std::string DESTINATION_LABEL;
        const static std::string HEALTH_POINTS_LABEL;
        const static std::string LAST_SHOOT_FRAME_COUNT_LABEL;

        const xg::Guid _singuityId;
        const xg::Guid _playerId;
        const CommunicatedVector2D _position;
        const CommunicatedVector2D _speed;
        const Option<CommunicatedSinguityDestination> _destination;
        const double _healthPoint;
        const long long _lastShootFameCount;
    };
}