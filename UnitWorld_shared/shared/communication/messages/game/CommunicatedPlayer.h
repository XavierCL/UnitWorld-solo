#pragma once

#include "commons/Guid.hpp"

namespace uw
{
    class CommunicatedPlayer
    {
    public:
        CommunicatedPlayer(const xg::Guid& playerId);

        std::string toJson() const;

        static CommunicatedPlayer fromJson(std::string jsonData);

        xg::Guid playerId() const;

    private:
        const xg::Guid _playerId;
    };
}