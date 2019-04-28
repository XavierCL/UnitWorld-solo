#include "lobby/ClientRoom.h"

#include "communications/ServerConnector.h"

#include "commons/Logger.hpp"

#include <thread>
#include <iostream>

int main()
{
    Logger::registerInfo([](const std::string& message) {
        std::cout << message;
    });
    ClientRoom clientRoom(2);
    Logger::info("Waiting for connections...\n");
    ServerConnector serverConnector(
        ConnectionInfo("127.0.0.1", "52124"),
        [&clientRoom](std::shared_ptr<CommunicationHandler> communicationHandler) {
        clientRoom.addClient(communicationHandler);
    }
    );
    serverConnector.acceptBlocking();
}
