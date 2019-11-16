#include "ServerGame.h"

#include "shared/game/GameManager.h"
#include "shared/game/physics/collisions/KdtreeCollisionDetectorFactory.h"
#include "shared/configuration/ConfigurationManager.h"

#include "communications/ServerConnector.h"

#include "commons/Logger.hpp"
#include "commons/CollectionPipe.h"

#include <iostream>
#include <fstream>

using namespace uw;

int main()
{
    std::ofstream traceLogFile("trace_server.log", std::ios_base::app);
    Logger::registerError([](const std::string& errorMessage) {
        std::cout << "ERROR: " << errorMessage << std::endl;
    });
    Logger::registerInfo([](const std::string& message) {
        std::cout << "INFO: " << message << std::endl;
    });
    Logger::registerTrace([&traceLogFile](const auto& traceMessage) {

        auto currentTime = std::time(nullptr);
        auto localTime = *std::localtime(&currentTime);

        traceLogFile << std::put_time(&localTime, "%Y-%m-%dT%H-%M-%S") << ": " << traceMessage << std::endl;
        traceLogFile.flush();
    });

    const std::string DEFAULT_SERVER_PORT("52124");

    try
    {
        const ConfigurationManager configurationManager("config.json");

        const std::string serverPort = configurationManager.serverPortOrDefault(DEFAULT_SERVER_PORT);
        const std::vector<std::vector<Vector2D>> firstSpawners = configurationManager.firstSpawners();
        const auto singuitiesBySpawner = configurationManager.singuitiesBySpawner();

        const auto gameManager(std::make_shared<GameManager>());

        const auto naiveCollisionDetectorFactory(std::make_shared<KdtreeCollisionDetectorFactory>());
        const auto physicsManager(std::make_shared<PhysicsManager>(gameManager, naiveCollisionDetectorFactory, std::make_shared<PhysicsStats>()));

        const auto messageSerializer(std::make_shared<MessageSerializer>());
        const auto physicsCommunicationAssembler(std::make_shared<PhysicsCommunicationAssembler>());
        const auto clientsGameSender(std::make_shared<ClientsGameSender>(gameManager, messageSerializer, physicsCommunicationAssembler));

        const auto gameReceiver(std::make_shared<GameReceiver>(gameManager, physicsCommunicationAssembler));
        const auto clientsReceiver(std::make_shared<ClientsReceiver>(messageSerializer, gameReceiver));

        ServerGame serverGame(gameManager, physicsManager, clientsGameSender, clientsReceiver, firstSpawners, singuitiesBySpawner);
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
    catch (std::exception error)
    {
        Logger::error("Error catched in the global error handler. Error message is: " + std::string(error.what()));
    }

    return 0;
}
