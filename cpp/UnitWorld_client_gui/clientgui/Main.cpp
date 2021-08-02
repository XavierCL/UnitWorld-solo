#include "ClientGame.h"

#include "graphics/canvas/SFMLCanvas.h"
#include "graphics/canvas/CanvasTransactionGenerator.h"
#include "userControls/WindowInputs.h"

#include "WindowManager.h"

#include "shared/game/physics/collisions/KdtreeCollisionDetectorFactory.h"
#include "shared/game/geometry/Rectangle.h"

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
    std::ofstream errorLogFile("error_client_gui.log", std::ios_base::app);
    std::ofstream traceLogFile("trace_client_gui.log", std::ios_base::app);
    Logger::registerError([&errorLogFile](const auto& errorMessage) {

        auto currentTime = std::time(nullptr);
        auto localTime = *std::localtime(&currentTime);

        errorLogFile << std::put_time(&localTime, "%Y-%m-%dT%H-%M-%S") << ": " << errorMessage << std::endl;
        errorLogFile.flush();
    });
    Logger::registerTrace([&traceLogFile](const auto& traceMessage) {

        auto currentTime = std::time(nullptr);
        auto localTime = *std::localtime(&currentTime);

        traceLogFile << std::put_time(&localTime, "%Y-%m-%dT%H-%M-%S") << ": " << traceMessage << std::endl;
        traceLogFile.flush();
    });

    const std::string WINDOW_TITLE("Unit World client GUI");
    const std::string DEFAULT_SERVER_IP("127.0.0.1");
    const std::string DEFAULT_SERVER_PORT("52124");
    const double DEFAULT_WORLD_ABSOLUTE_WIDTH(2000);
    const double DEFAULT_WORLD_ABSOLUTE_HEIGHT(1200);
    const double DEFAULT_SIDE_PANEL_WIDTH_RATIO(0.03);
    const double DEFAULT_TRANSLATION_PIXEL_PER_FRAME(10.0);
    const double DEFAULT_SCROLL_RATIO_PER_TICK(0.2);

    try
    {
        const ConfigurationManager configurationManager("config.json");

        const std::string serverIp = configurationManager.serverIpOrDefault(DEFAULT_SERVER_IP);
        const std::string serverPort = configurationManager.serverPortOrDefault(DEFAULT_SERVER_PORT);
        const double worldAbsoluteWidth = configurationManager.worldAbsoluteWidth(DEFAULT_WORLD_ABSOLUTE_WIDTH);
        const double worldAbsoluteHeight = configurationManager.worldAbsoluteHeight(DEFAULT_WORLD_ABSOLUTE_HEIGHT);
        const double sidePanelWidthRatio = configurationManager.sidePanelWidthRatio(DEFAULT_SIDE_PANEL_WIDTH_RATIO);
        const double translationPixelPerFrame = configurationManager.translationPixelPerFrame(DEFAULT_TRANSLATION_PIXEL_PER_FRAME);
        const double scrollRatioPerTick = configurationManager.scrollRatioPerTick(DEFAULT_SCROLL_RATIO_PER_TICK);

        const unsigned int GRAPHICS_FRAME_PER_SECOND(26);

        auto window(std::make_shared<sf::RenderWindow>(sf::VideoMode::getFullscreenModes().front(), WINDOW_TITLE, sf::Style::Fullscreen));

        ClientConnector(ConnectionInfo(serverIp, serverPort), [&window, &GRAPHICS_FRAME_PER_SECOND, &worldAbsoluteWidth, &worldAbsoluteHeight, &sidePanelWidthRatio, &translationPixelPerFrame, &scrollRatioPerTick](const std::shared_ptr<CommunicationHandler>& connectionHandler) {

            const auto gameManager(std::make_shared<GameManager>());

            const auto kdtreeCollisionDetectorFactory(std::make_shared<KdtreeCollisionDetectorFactory>());
            const auto physicsManager(std::make_shared<PhysicsManager>(gameManager, kdtreeCollisionDetectorFactory, std::make_shared<PhysicsStats>()));

            const auto camera(std::make_shared<Camera>(worldAbsoluteWidth, worldAbsoluteHeight, uw::Rectangle(Vector2D(0.0, 0.0), Vector2D(window->getSize().x, window->getSize().y)), sidePanelWidthRatio, translationPixelPerFrame, scrollRatioPerTick));
            const auto cameraRelativeGameManager(std::make_shared<CameraRelativeGameManager>(camera));
            const auto physicsCommunicationAssembler(std::make_shared<PhysicsCommunicationAssembler>());
            const auto messageSerializer(std::make_shared<MessageSerializer>());
            const auto serverCommander(std::make_shared<ServerCommander>(connectionHandler, physicsCommunicationAssembler, messageSerializer));
            const auto userControlState(std::make_shared<UserControlState>(gameManager, cameraRelativeGameManager, serverCommander));

            const auto gameDrawer(std::make_shared<GameDrawer>(gameManager, userControlState, cameraRelativeGameManager));
            const auto sfmlCanvas(std::make_shared<uw::SFMLCanvas>(window));
            const auto canvasTransactionGenerator(std::make_shared<CanvasTransactionGenerator>(sfmlCanvas));
            const auto windowInputs(std::make_shared<WindowInputs>(userControlState, camera, uw::Rectangle(Vector2D(0.0, 0.0), Vector2D(window->getSize().x, window->getSize().y))));
            const auto windowManager(std::make_shared<WindowManager>(GRAPHICS_FRAME_PER_SECOND, gameDrawer, canvasTransactionGenerator, window, windowInputs, std::make_shared<WindowStats>()));

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