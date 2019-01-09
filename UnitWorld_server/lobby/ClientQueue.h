#pragma once

#include "communications\CommunicationHandler.h"

#include "asio/buffer.hpp"

class ClientQueue
{
public:
	
	ClientQueue(const std::vector<std::shared_ptr<CommunicationHandler>>& communicationHandlers, const std::function<void(const asio::mutable_buffer)>& gameNotifier) :
		_communicationHandlers(communicationHandlers),
		_gameNotifier(gameNotifier)
	{
		for (auto& communicationHandler : communicationHandlers)
		{
			asio::mutable_buffer buffer;
			
		}
	}

private:

	void clientDataReceived(const asio::mutable_buffer& receivedBuffer)
	{
		_gameNotifier
	}

	std::vector<std::shared_ptr<CommunicationHandler>> _communicationHandlers;
	std::vector<asio::mutable_buffer> _communicationBuffers;
	std::function<void(asio::mutable_buffer)> _gameNotifier;
};
