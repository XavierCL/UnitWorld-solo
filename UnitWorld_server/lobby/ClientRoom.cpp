#include "ClientRoom.h"

#include "commons/Logger.hpp"

using namespace uw;

ClientRoom::ClientRoom(const size_t numberOfNeededClients) :
    _numberOfNeededClients(numberOfNeededClients)
{}

void ClientRoom::addClient(std::shared_ptr<CommunicationHandler> communicationHandler)
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
        // Starts the game with the connected clients
    }
}

size_t ClientRoom::numberOfClients() const
{
    return _communicationHandlers.size();
}