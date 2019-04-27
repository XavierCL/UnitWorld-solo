#pragma once

#include "ConnectionInfo.h"
#include "CommunicationHandler.h"

#include <asio.hpp>

#include <functional>

class ServerConnector
{
public:

	ServerConnector(const ConnectionInfo& connectionInfo, const std::function<void(std::shared_ptr<CommunicationHandler>)>& clientConnectedCallback) :
		_acceptor(_ioService, endpoint(_ioService, connectionInfo)),
		_clientConnectedCallback(clientConnectedCallback)
	{}

	void acceptBlocking()
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

	~ServerConnector()
	{
		_acceptor.close();
	}

private:

	static asio::ip::tcp::endpoint endpoint(asio::io_service& ioService, const ConnectionInfo& connectionInfo)
	{
		asio::ip::tcp::resolver resolver(ioService);
		return *resolver.resolve(asio::ip::tcp::resolver::query(connectionInfo.host(), connectionInfo.port()));
	}

	asio::io_service _ioService;
	asio::ip::tcp::acceptor _acceptor;
	std::function<void(std::shared_ptr<CommunicationHandler>)> _clientConnectedCallback;
};
