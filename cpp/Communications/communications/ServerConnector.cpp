#include "ServerConnector.h"

using namespace uw;

ServerConnector::ServerConnector(const ConnectionInfo& connectionInfo, const std::function<void(std::shared_ptr<CommunicationHandler>)>& clientConnectedCallback) :
    _acceptor(_ioService, endpoint(_ioService, connectionInfo)),
    _clientConnectedCallback(clientConnectedCallback)
{}

void ServerConnector::acceptBlocking()
{
    auto newClientSocket(std::make_shared<asio::ip::tcp::socket>(_ioService));
    asio::ip::tcp::endpoint newClientEndpoint;
    _acceptor.async_accept(*newClientSocket, newClientEndpoint, [this, &newClientSocket, &newClientEndpoint](const asio::error_code& error) {
        if (!error)
        {
            _clientConnectedCallback(std::make_shared<CommunicationHandler>(newClientSocket, newClientEndpoint));

            acceptBlocking();
        }
    });
    _ioService.run();
}

ServerConnector::~ServerConnector()
{
    _acceptor.close();
}

asio::ip::tcp::endpoint ServerConnector::endpoint(asio::io_service& ioService, const ConnectionInfo& connectionInfo)
{
    asio::ip::tcp::resolver resolver(ioService);
    return *resolver.resolve(asio::ip::tcp::resolver::query(connectionInfo.host(), connectionInfo.port()));
}