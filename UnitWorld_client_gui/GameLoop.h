#pragma once

#include "SFML\Window.hpp"

namespace uw
{
	class GameLoop
	{
	public:
		const bool GAME_FINISHED = true;
		const bool GAME_CONTINUES = false;

		GameLoop(sf::Window& mainWindow);
		bool loop();
		~GameLoop();
	private:
		sf::Window& _mainWindow;
	};
}