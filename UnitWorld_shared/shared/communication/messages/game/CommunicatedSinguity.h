#pragma once

#include "CommunicatedVector2D.h"

#include "commons/Guid.hpp"
#include "commons/Option.hpp"

namespace uw
{
    class CommunicatedSinguity
    {
    public:
        CommunicatedSinguity(const xg::Guid& singuityId, const xg::Guid& playerId, const CommunicatedVector2D& position, const CommunicatedVector2D& speed, const Option<CommunicatedVector2D>& destination);

        std::string toJson() const;

        static CommunicatedSinguity fromJson(const std::string& jsonData);

        xg::Guid singuityId() const;
        xg::Guid playerId() const;

        CommunicatedVector2D position() const;
        CommunicatedVector2D speed() const;
        Option<CommunicatedVector2D> destination() const;

    private:
        const xg::Guid _singuityId;
        const xg::Guid _playerId;
        const CommunicatedVector2D _position;
        const CommunicatedVector2D _speed;
        const Option<CommunicatedVector2D> _destination;
    };
}