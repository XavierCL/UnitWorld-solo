#include "ServerGame.h"

#include "communications/ServerConnector.h"

#include "commons/Logger.hpp"

#include <thread>
#include <iostream>

using namespace uw;

int main()
{
    Logger::registerInfo([](const std::string& message) {
        std::cout << message;
    });

    ServerGame serverGame(5);
    Logger::info("Waiting for connections...\n");
    ServerConnector serverConnector(ConnectionInfo("127.0.0.1", "52124"),
        [&serverGame](std::shared_ptr<CommunicationHandler> communicationHandler) {
            Logger::info("Connection from " + communicationHandler->prettyName());
            serverGame.addClient(communicationHandler);
        }
    );
    serverConnector.acceptBlocking();
}
