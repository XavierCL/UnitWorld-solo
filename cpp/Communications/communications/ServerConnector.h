#pragma once

#include "ConnectionInfo.h"
#include "CommunicationHandler.h"

#include <asio.hpp>

#include <functional>

namespace uw
{
    class ServerConnector
    {
    public:

        ServerConnector(const ConnectionInfo& connectionInfo, const std::function<void(std::shared_ptr<CommunicationHandler>)>& clientConnectedCallback);

        void acceptBlocking();

        ~ServerConnector();

    private:

        static asio::ip::tcp::endpoint endpoint(asio::io_service& ioService, const ConnectionInfo& connectionInfo);

        asio::io_service _ioService;
        asio::ip::tcp::acceptor _acceptor;
        std::function<void(std::shared_ptr<CommunicationHandler>)> _clientConnectedCallback;
    };
}