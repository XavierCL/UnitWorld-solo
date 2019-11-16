#pragma once

#include "ConnectionInfo.h"
#include "CommunicationHandler.h"

#include <functional>

namespace uw
{
    class ClientConnector
    {
    public:
        ClientConnector(const ConnectionInfo& connectionInfo, const std::function<void(std::shared_ptr<CommunicationHandler>)>& serverConnectedCallback, const std::function<void(std::error_code)>& errorCallback);
    };
}