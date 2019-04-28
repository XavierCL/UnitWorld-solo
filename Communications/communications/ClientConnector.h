#pragma once

#include "ConnectionInfo.h"
#include "CommunicationHandler.h"

#include <asio.hpp>

#include <functional>

class ClientConnector
{
public:
    ClientConnector(const ConnectionInfo& connectionInfo, const std::function<void(std::shared_ptr<CommunicationHandler>)>& serverConnectedCallback)
    {
        asio::io_service ioService;
        asio::ip::tcp::resolver resolver(ioService);
        const auto endpoints(resolver.resolve(asio::ip::tcp::resolver::query(connectionInfo.host(), connectionInfo.port())));
        auto socket(std::make_shared<asio::ip::tcp::socket>(ioService));
        const auto connectedEndpoint(asio::connect(*socket, endpoints));
        serverConnectedCallback(std::make_shared<CommunicationHandler>(socket, connectedEndpoint));
    }
};
