#pragma once

#include "commons/Guid.hpp"

#include "communications/CommunicationHandler.h"

namespace uw
{
    class PlayerClient
    {
    public:
        PlayerClient(xg::Guid playerId, std::shared_ptr<CommunicationHandler> client) :
            _playerId(playerId),
            _client(client)
        {}

        xg::Guid playerId() const
        {
            return _playerId;
        }

        std::shared_ptr<CommunicationHandler> client() const
        {
            return _client;
        }

    private:
        const xg::Guid _playerId;
        const std::shared_ptr<CommunicationHandler> _client;
    };
}