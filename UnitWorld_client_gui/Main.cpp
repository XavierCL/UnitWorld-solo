#include "game/GameLoop.h"
#include "graphics/canvas/SFMLCanvas.h"
#include "graphics/shared/SharedSFMLDisplayDrawer.h"

#include "communications/ClientConnector.h"

#include <SFML/Graphics.hpp>

#include <thread>

int main()
{
	ClientConnector(ConnectionInfo("127.0.0.1", "52124"), [](const std::shared_ptr<CommunicationHandler>& connectionHandler) {
		OutputDebugStringA("Server connected!");
	});

	unsigned int screenWidth = 800;
	unsigned int screenHeight = 600;

	sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Unit World");

	uw::SFMLCanvas canvas(window);
	auto sharedHandler(std::make_shared<uw::SharedSFMLDislayDrawer>(canvas));
	uw::GameLoop game(sharedHandler);

	window.setActive(false);

	std::thread gameThread([&window, &game]{
		while(window.isOpen())
		{
			auto counter1 = std::chrono::steady_clock::now();
			game.loop();
			auto counter2 = std::chrono::steady_clock::now();
			int remainingSleepTime = 30 - (int)std::chrono::duration <double, std::milli>(counter2 - counter1).count();
			if(remainingSleepTime >= 1)
				std::this_thread::sleep_for(std::chrono::milliseconds(remainingSleepTime));
		}
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

		sharedHandler->tryClose();
	}

	gameThread.join();
	return EXIT_SUCCESS;
}
