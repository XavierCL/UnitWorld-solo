#include "ClientConnector.h"

#include <asio.hpp>

using namespace uw;

ClientConnector::ClientConnector(const ConnectionInfo& connectionInfo, const std::function<void(std::shared_ptr<CommunicationHandler>)>& serverConnectedCallback, const std::function<void(std::error_code)>& errorCallback)
{
    asio::io_service ioService;
    asio::ip::tcp::resolver resolver(ioService);
    const auto endpoints(resolver.resolve(asio::ip::tcp::resolver::query(connectionInfo.host(), connectionInfo.port())));
    auto socket(std::make_shared<asio::ip::tcp::socket>(ioService));

    asio::error_code connectionErrorCode;
    const auto connectedEndpoint(asio::connect(*socket, endpoints, connectionErrorCode));

    if (connectionErrorCode)
    {
        errorCallback(connectionErrorCode);
    }
    else
    {
        serverConnectedCallback(std::make_shared<CommunicationHandler>(socket, connectedEndpoint));
    }
}