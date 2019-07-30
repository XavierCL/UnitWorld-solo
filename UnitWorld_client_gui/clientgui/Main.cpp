#include "ClientGame.h"

#include "graphics/canvas/SFMLCanvas.h"
#include "graphics/canvas/CanvasTransactionGenerator.h"

#include "shared/communication/MessageWrapper.h"
#include "shared/communication/CompleteGameStateMessage.h"

#include "communications/ClientConnector.h"

#include <SFML/Graphics.hpp>

#include <thread>

using namespace uw;

int main()
{
    const std::string WINDOW_TITLE("Unit World client GUI");
    const std::string SERVER_IP("127.0.0.1");
    const std::string SERVER_PORT("52124");

    const unsigned int GRAPHICS_FRAME_PER_SECOND(10);

    const unsigned int PHYSICS_FRAME_PER_SECOND(30);

    sf::RenderWindow window(sf::VideoMode::getFullscreenModes().front(), WINDOW_TITLE);

    window.setActive(false);

    ClientConnector(ConnectionInfo(SERVER_IP, SERVER_PORT), [&window, &GRAPHICS_FRAME_PER_SECOND, &PHYSICS_FRAME_PER_SECOND](const std::shared_ptr<CommunicationHandler>& connectionHandler) {

        auto messageSerializer(std::make_shared<MessageSerializer>());
        std::vector<MessageWrapper> receivedMessages;
        while ((receivedMessages = messageSerializer->deserialize(connectionHandler->receive())).empty()) {}

        const auto firstCompleteGameStateMessage(std::dynamic_pointer_cast<const CompleteGameStateMessage>(receivedMessages.front().innerMessage()));
        const auto currentPlayerId(firstCompleteGameStateMessage->getCurrentPlayerId());
        const auto currentPlayer(std::make_shared<Player>(currentPlayerId, std::vector<std::shared_ptr<Singuity>>()));

        auto sharedHandler(std::make_shared<uw::CanvasTransactionGenerator>(std::make_shared<uw::SFMLCanvas>(window)));

        ClientGame clientGame(GRAPHICS_FRAME_PER_SECOND, PHYSICS_FRAME_PER_SECOND, currentPlayer, connectionHandler, sharedHandler, messageSerializer);

        clientGame.startSync();
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