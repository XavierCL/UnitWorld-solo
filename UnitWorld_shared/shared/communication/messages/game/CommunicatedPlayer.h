#pragma once

#include "commons/Guid.hpp"

#include <nlohmann/json.hpp>

namespace uw
{
    class CommunicatedPlayer
    {
    public:
        CommunicatedPlayer(const xg::Guid& playerId);

        nlohmann::json toJson() const;

        static CommunicatedPlayer fromJson(const nlohmann::json& jsonData);

        xg::Guid playerId() const;

    private:
        const xg::Guid _playerId;
    };
}