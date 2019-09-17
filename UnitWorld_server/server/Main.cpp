#include "ServerGame.h"

#include "shared/game/GameManager.h"

#include "communications/ServerConnector.h"

#include "commons/Logger.hpp"
#include "commons/CollectionPipe.h"

#include <iostream>

using namespace uw;

int main()
{
    Logger::registerInfo([](const std::string& message) {
        std::cout << message;
    });

    const auto gameManager(std::make_shared<GameManager>());

    const auto messageSerializer(std::make_shared<MessageSerializer>());
    const auto physicsCommunicationAssembler(std::make_shared<PhysicsCommunicationAssembler>());
    const auto clientsGameSender(std::make_shared<ClientsGameSender>(gameManager, messageSerializer, physicsCommunicationAssembler));

    const auto gameReceiver(std::make_shared<GameReceiver>(gameManager, physicsCommunicationAssembler));
    const auto clientsReceiver(std::make_shared<ClientsReceiver>(messageSerializer, gameReceiver));

    ServerGame serverGame(gameManager, clientsGameSender, clientsReceiver);
    serverGame.startAsync();

    Logger::info("Waiting for connections...\n");
    ServerConnector serverConnector(ConnectionInfo("127.0.0.1", "52124"),
        [&serverGame](std::shared_ptr<CommunicationHandler> communicationHandler) {
            Logger::info("Connection from " + communicationHandler->prettyName());
            serverGame.addClient(communicationHandler);
        }
    );
    serverConnector.acceptBlocking();
}
