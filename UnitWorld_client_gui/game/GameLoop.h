#pragma once

#include "../graphics/shared/SharedDrawer.h"
#include "../graphics/canvas/DrawingCanvas.h"

#include "SFML/Graphics.hpp"

namespace uw
{
	class GameLoop
	{
	public:

		GameLoop(SharedDrawer& drawer) :
			_drawer(drawer),
			temp1(300)
		{}
		void loop()
		{
			auto gameState = handlePhysics();
			handleGraphics(gameState);
		}
	private:
		static const bool GAME_STATE_ENDED = true;
		static const bool GAME_STATE_RUNNING = false;

		bool handlePhysics()
		{
			++temp1;
			return temp1 >= 600;
		}

		void handleGraphics(const bool& gameState)
		{
			if (gameState)
			{
				_drawer.requestClosure();
			}
			else
			{
				_drawer.tryDrawingTransaction([this](DrawingCanvas& canvas)
				{
					auto r = sf::RectangleShape(sf::Vector2f(1000, 1000));
					r.setFillColor(sf::Color(sf::Uint8(128), sf::Uint8(128), sf::Uint8(200)));
					r.setPosition(temp1, temp1);
					canvas.draw(r);
				});
			}
		}

		int temp1;
		SharedDrawer& _drawer;
	};
}