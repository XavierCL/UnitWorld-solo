#pragma once

#include "game/play/Player.h"
#include "game/play/Singuity.h"

#include "graphics/GraphicalSinguity.h"

#include "../graphics/shared/SharedDrawer.h"
#include "../graphics/canvas/DrawingCanvas.h"

#include "SFML/Graphics.hpp"

namespace uw
{
	class GameLoop
	{
	public:

		GameLoop(SharedDrawer& drawer) :
			_drawer(drawer)
		{
			_currentPlayer = new Player();
			_currentPlayer->addSinguity(new Singuity(Vector2(150, 150)));
			_currentPlayer->addSinguity(new Singuity(Vector2(200, 150)));
			_currentPlayer->selectMobileUnitsInArea(Rectangle(Vector2(0, 0), Vector2(200, 200)));
			_currentPlayer->setSelectedMobileUnitsDestination(Vector2(400, 400));
		}

		~GameLoop()
		{
			delete _currentPlayer;
		}

		void loop()
		{
			auto gameIsDone = handlePhysics();
			handleGraphics(gameIsDone);
		}
	private:
		static const bool GAME_STATE_ENDED = true;
		static const bool GAME_STATE_RUNNING = false;

		bool handlePhysics()
		{
			_currentPlayer->actualize();
			return false;
		}

		void handleGraphics(const bool& gameIsDone)
		{
			if (gameIsDone)
			{
				_drawer.requestClosure();
			}
			else
			{
				_drawer.tryDrawingTransaction([this](DrawingCanvas& canvas)
				{
					for(auto singuity: _currentPlayer->singuities())
					{
						canvas.draw((GraphicalSinguity(*singuity).drawable()));
					}
				});
			}
		}

		SharedDrawer& _drawer;
		Player* _currentPlayer;
	};
}