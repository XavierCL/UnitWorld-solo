#include "ClientGame.h"

#include "graphics/canvas/SFMLCanvas.h"
#include "graphics/canvas/CanvasTransactionGenerator.h"

#include "graphics/WindowManager.h"

#include "shared/game/physics/collisions/NaiveCollisionDetectorFactory.h"

#include "shared/configuration/ConfigurationManager.h"

#include "communications/ClientConnector.h"

#include "commons/Logger.hpp"

#include <SFML/Graphics.hpp>

#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>

using namespace uw;

int main()
{
    Logger::registerError([](const auto& errorMessage) {

        auto currentTime = std::time(nullptr);
        auto localTime = *std::localtime(&currentTime);

        std::ofstream outputFile("error.log", std::ios_base::app);
        outputFile << std::put_time(&localTime, "%Y-%m-%dT%H-%M-%S") << ": " << errorMessage << std::endl;
    });

    const std::string WINDOW_TITLE("Unit World client GUI");
    const std::string DEFAULT_SERVER_IP("127.0.0.1");
    const std::string DEFAULT_SERVER_PORT("52124");

    try
    {
        const ConfigurationManager configurationManager("config.json");

        const std::string serverIp = configurationManager.serverIpOrDefault(DEFAULT_SERVER_IP);
        const std::string serverPort = configurationManager.serverPortOrDefault(DEFAULT_SERVER_PORT);

        const unsigned int GRAPHICS_FRAME_PER_SECOND(30);

        auto window(std::make_shared<sf::RenderWindow>(sf::VideoMode::getFullscreenModes().front(), WINDOW_TITLE));

        ClientConnector(ConnectionInfo(serverIp, serverPort), [&window, &GRAPHICS_FRAME_PER_SECOND](const std::shared_ptr<CommunicationHandler>& connectionHandler) {

            const auto gameManager(std::make_shared<GameManager>());

            const auto naiveCollisionDetectorFactory(std::make_shared<NaiveCollisionDetectorFactory>());
            const auto physicsManager(std::make_shared<PhysicsManager>(gameManager, naiveCollisionDetectorFactory));

            const auto physicsCommunicationAssembler(std::make_shared<PhysicsCommunicationAssembler>());
            const auto messageSerializer(std::make_shared<MessageSerializer>());
            const auto serverCommander(std::make_shared<ServerCommander>(connectionHandler, physicsCommunicationAssembler, messageSerializer));
            const auto userControlState(std::make_shared<UserControlState>(gameManager, serverCommander));

            const auto gameDrawer(std::make_shared<GameDrawer>(gameManager, userControlState));
            const auto sfmlCanvas(std::make_shared<uw::SFMLCanvas>(window));
            const auto canvasTransactionGenerator(std::make_shared<CanvasTransactionGenerator>(sfmlCanvas));
            const auto windowManager(std::make_shared<WindowManager>(GRAPHICS_FRAME_PER_SECOND, gameDrawer, canvasTransactionGenerator, window, userControlState));

            const auto serverReceiver(std::make_shared<ServerReceiver>(connectionHandler, gameManager, physicsCommunicationAssembler, messageSerializer));

            const auto clientGame(std::make_shared<ClientGame>(gameManager, physicsManager, windowManager, serverReceiver));

            clientGame->startSync();
        }, [](const std::error_code& errorCode) {
            Logger::error("Could not connect to the server. Error code #" + std::to_string(errorCode.value()) + ". Error message: " + errorCode.message());
        });
    }
    catch (std::exception error)
    {
        Logger::error("Error catched in the global handler. Message is " + std::string(error.what()));
        throw;
    }

    return EXIT_SUCCESS;
}