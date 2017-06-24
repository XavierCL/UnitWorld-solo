#include "game/GameLoop.h"
#include "graphics/canvas/SFMLCanvas.h"
#include "graphics/shared/SharedSFMLDisplayDrawer.h"

#include <SFML/Graphics.hpp>

#include <thread>

int WinMain()
{
	unsigned int screenWidth = 800;
	unsigned int screenHeight = 600;

	sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Unit World");

	uw::SFMLCanvas canvas(window);
	uw::SharedSFMLDislayDrawer sharedHandler(canvas);
	uw::GameLoop game((uw::SharedDrawer&)sharedHandler);

	window.setActive(false);

	std::thread gameThread([&window, &game](){
		while(window.isOpen())
		{
			game.loop();
			std::this_thread::sleep_for(std::chrono::milliseconds(30));
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

		sharedHandler.tryClose();
	}

	gameThread.join();
	return EXIT_SUCCESS;
}