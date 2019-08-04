#include "ClientGame.h"

#include "graphics/canvas/SFMLCanvas.h"
#include "graphics/canvas/CanvasTransactionGenerator.h"

#include "graphics/WindowManager.h"

#include "communications/ClientConnector.h"

#include <SFML/Graphics.hpp>

using namespace uw;

int main()
{
    const std::string WINDOW_TITLE("Unit World client GUI");
    const std::string SERVER_IP("127.0.0.1");
    const std::string SERVER_PORT("52124");

    const unsigned int GRAPHICS_FRAME_PER_SECOND(30);

    auto window(std::make_shared<sf::RenderWindow>(sf::VideoMode::getFullscreenModes().front(), WINDOW_TITLE));

    ClientConnector(ConnectionInfo(SERVER_IP, SERVER_PORT), [&window, &GRAPHICS_FRAME_PER_SECOND](const std::shared_ptr<CommunicationHandler>& connectionHandler) {

        const auto gameManager(std::make_shared<GameManager>());

        const auto gameDrawer(std::make_shared<GameDrawer>(gameManager));
        const auto sfmlCanvas(std::make_shared<uw::SFMLCanvas>(window));
        const auto canvasTransactionGenerator(std::make_shared<CanvasTransactionGenerator>(sfmlCanvas));
        const auto windowManager(std::make_shared<WindowManager>(GRAPHICS_FRAME_PER_SECOND, gameDrawer, canvasTransactionGenerator, window));

        const auto messageSerializer(std::make_shared<MessageSerializer>());
        const auto serverReceiver(std::make_shared<ServerReceiver>(connectionHandler, gameManager, messageSerializer));

        const auto clientGame(std::make_shared<ClientGame>(gameManager, windowManager, serverReceiver));

        clientGame->startSync();
    });

    return EXIT_SUCCESS;
}