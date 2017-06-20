#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "GameLoop.h"

int main()
{
	unsigned int screenWidth = 800;
	unsigned int screenHeight = 600;
	// Create the main window
	sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Unit World");
	//Create the game instance
	uw::GameLoop gameLoop(window);
	// Start the game loop
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
		// Clear screen
		window.clear();
		//Game loop
		if(gameLoop.loop();
		// Update the window
		window.display();
	}
	return EXIT_SUCCESS;
}