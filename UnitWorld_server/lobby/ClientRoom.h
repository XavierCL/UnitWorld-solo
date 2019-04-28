#pragma once

#include "communications/CommunicationHandler.h"

namespace uw
{
    class ClientRoom {
    public:

        ClientRoom(const size_t numberOfNeededClients);

        void addClient(std::shared_ptr<CommunicationHandler> communicationHandler);

        size_t numberOfClients() const;

    private:

        std::vector<std::shared_ptr<CommunicationHandler>> _communicationHandlers;
        const size_t _numberOfNeededClients;
    };
}