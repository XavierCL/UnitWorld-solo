#pragma once

#include "SFML\Window.hpp"

namespace uw
{
	class GameLoop
	{
	public:
		const bool GAME_FINISHED = true;
		const bool GAME_CONTINUES = false;

		GameLoop(sf::Window& mainWindow): _mainWindow(mainWindow)
		{

		}
		bool loop()
		{
			return true;
		}
		~GameLoop()
		{

		}
	private:
		sf::Window& _mainWindow;
	};
}