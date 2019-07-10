#pragma once

#include "CommunicatedVector2D.h"

#include "commons/Guid.hpp"

#include <nlohmann/json.hpp>

namespace uw
{
    class CommunicatedSinguity
    {
    public:
        CommunicatedSinguity(const xg::Guid& singuityId, const xg::Guid& playerId, const CommunicatedVector2D& position, const CommunicatedVector2D& speed, const Option<CommunicatedVector2D>& destination):
            _singuityId(singuityId),
            _playerId(playerId),
            _position(position),
            _speed(speed),
            _destination(destination)
        {}

        std::string toJson() const
        {
            nlohmann::json jsonData = {
                {"id", _singuityId.str()},
                {"playerId", _playerId.str()},
                {"position", _position.toJson()},
                {"speed", _speed.toJson()},
                {"destination", _destination.map([](const auto& destination) { return destination.toJson(); }).getOrElse("none")}
            };

            return jsonData;
        }

    private:
        const xg::Guid _singuityId;
        const xg::Guid _playerId;
        const CommunicatedVector2D _position;
        const CommunicatedVector2D _speed;
        const Option<CommunicatedVector2D> _destination;
    };
}