#pragma once

#include "communications\CommunicationHandler.h"

#include "commons\Logger.hpp"

class ClientRoom {
public:

	ClientRoom(const size_t numberOfNeededClients) :
		_numberOfNeededClients(numberOfNeededClients)
	{}

	void addClient(std::shared_ptr<CommunicationHandler> communicationHandler)
	{
		_communicationHandlers.push_back(communicationHandler);
		Logger::info("Client " + communicationHandler->prettyName() + " connected\n");

		if (_numberOfNeededClients - numberOfClients() == 1)
		{
			// See the missing 's' in 'client'
			Logger::info("Waiting for 1 more client\n");
		}
		else if (numberOfClients() < _numberOfNeededClients)
		{
			Logger::info("Waiting for " + std::to_string(_numberOfNeededClients - numberOfClients()) + " more clients\n");
		}
		else
		{

		}
	}

	size_t numberOfClients() const
	{
		return _communicationHandlers.size();
	}

private:

	std::vector<std::shared_ptr<CommunicationHandler>> _communicationHandlers;
	const size_t _numberOfNeededClients;
};
