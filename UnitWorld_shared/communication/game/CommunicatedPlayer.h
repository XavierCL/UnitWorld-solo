#pragma once

#include "commons/Guid.hpp"

#include <nlohmann/json.hpp>

namespace uw
{
    class CommunicatedPlayer
    {
    public:
        CommunicatedPlayer(const xg::Guid& playerId):
            _playerId(playerId)
        {}

        std::string toJson() const
        {
            nlohmann::json jsonData = {
                {"id", _playerId.str()}
            };

            return jsonData;
        }

    private:
        const xg::Guid _playerId;
    };
}