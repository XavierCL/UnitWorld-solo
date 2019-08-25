#pragma once

#include "CommunicatedVector2D.h"

#include "commons/Guid.hpp"
#include "commons/Option.hpp"

namespace uw
{
    class CommunicatedSinguity
    {
    public:
        CommunicatedSinguity(const xg::Guid& singuityId, const xg::Guid& playerId, const CommunicatedVector2D& position, const CommunicatedVector2D& speed, const Option<CommunicatedVector2D>& destination, const bool& isBreakingForDestination, const double& healthPoint, const unsigned long long& lastShootTimestamp);

        std::string toJson() const;

        static CommunicatedSinguity fromJson(const std::string& jsonData);

        xg::Guid singuityId() const;
        xg::Guid playerId() const;

        CommunicatedVector2D position() const;
        CommunicatedVector2D speed() const;
        Option<CommunicatedVector2D> destination() const;
        bool isBreakingForDestination() const;
        double healthPoint() const;
        unsigned long long lastShootTimestamp() const;

    private:
        const xg::Guid _singuityId;
        const xg::Guid _playerId;
        const CommunicatedVector2D _position;
        const CommunicatedVector2D _speed;
        const Option<CommunicatedVector2D> _destination;
        const bool _isBreakingForDestination;
        const double _healthPoint;
        const unsigned long long _lastShootTimestamp;
    };
}