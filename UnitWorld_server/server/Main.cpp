#include "ServerGame.h"

#include "shared/game/GameManager.h"
#include "shared/game/physics/NaiveCollisionDetectorFactory.h"

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

    const auto naiveCollisionDetectorFactory(std::make_shared<NaiveCollisionDetectorFactory>());

    const auto gameManager(std::make_shared<GameManager>(naiveCollisionDetectorFactory));

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
            Logger::info("Connection from " + communicationHandler->prettyName() + "\n");
            serverGame.addClient(communicationHandler);
        }
    );
    serverConnector.acceptBlocking();
}