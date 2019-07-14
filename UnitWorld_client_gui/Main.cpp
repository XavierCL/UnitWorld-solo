#include "game/GameLoop.h"

#include "graphics/canvas/SFMLCanvas.h"
#include "graphics/canvas/CanvasTransactionGenerator.h"

#include "communications/ClientConnector.h"

#include <SFML/Graphics.hpp>

#include <thread>

using namespace uw;

int main()
{
    sf::RenderWindow window(sf::VideoMode::getFullscreenModes().front(), "Unit World client GUI");

    window.setActive(false);
    auto sharedHandler(std::make_shared<uw::CanvasTransactionGenerator>(std::make_shared<uw::SFMLCanvas>(window)));

    ClientConnector(ConnectionInfo("127.0.0.1", "52124"), [](const std::shared_ptr<CommunicationHandler>& connectionHandler) {
        
    });

    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }

    return EXIT_SUCCESS;
}