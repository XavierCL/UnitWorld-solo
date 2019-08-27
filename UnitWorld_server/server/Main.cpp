#include "ServerGame.h"

#include "shared/game/GameManager.h"
#include "shared/game/physics/NaiveCollisionDetectorFactory.h"
#include "shared/configuration/ConfigurationManager.h"

#include "communications/ServerConnector.h"

#include "commons/Logger.hpp"
#include "commons/CollectionPipe.h"

#include <iostream>

using namespace uw;

int main()
{
    Logger::registerInfo([](const std::string& message) {
        std::cout << "INFO: " << message << std::endl;
    });
    Logger::registerError([](const std::string& errorMessage) {
        std::cout << "ERROR: " << errorMessage << std::endl;
    });

    const std::string DEFAULT_SERVER_PORT("52124");

    const ConfigurationManager configurationManager("config.json");

    const std::string serverPort = configurationManager.serverPortOrDefault(DEFAULT_SERVER_PORT);

    const auto naiveCollisionDetectorFactory(std::make_shared<NaiveCollisionDetectorFactory>());

    const auto gameManager(std::make_shared<GameManager>(naiveCollisionDetectorFactory));

    const auto messageSerializer(std::make_shared<MessageSerializer>());
    const auto physicsCommunicationAssembler(std::make_shared<PhysicsCommunicationAssembler>());
    const auto clientsGameSender(std::make_shared<ClientsGameSender>(gameManager, messageSerializer, physicsCommunicationAssembler));

    const auto gameReceiver(std::make_shared<GameReceiver>(gameManager, physicsCommunicationAssembler));
    const auto clientsReceiver(std::make_shared<ClientsReceiver>(messageSerializer, gameReceiver));

    ServerGame serverGame(gameManager, clientsGameSender, clientsReceiver);
    serverGame.startAsync();

    Logger::info("Waiting for connections...");
    ServerConnector serverConnector(ConnectionInfo("0.0.0.0", serverPort),
        [&serverGame](std::shared_ptr<CommunicationHandler> communicationHandler) {
            Logger::info("Connection from " + communicationHandler->prettyName());
            serverGame.addClient(communicationHandler);
        }
    );
    serverConnector.acceptBlocking();
}
