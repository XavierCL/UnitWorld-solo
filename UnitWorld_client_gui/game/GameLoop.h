#pragma once

#include "game/play/Player.h"
#include "game/play/Singuity.h"

#include "../graphics/units/GraphicalSinguity.h"

#include "../graphics/canvas/CanvasTransactionGenerator.h"
#include "../graphics/canvas/SFMLDrawingCanvas.h"

#include "SFML/Graphics.hpp"

namespace uw
{
    class GameLoop
    {
    public:

        GameLoop(std::shared_ptr<CanvasTransactionGenerator> canvasTransactionGenerator) :
            _canvasTransactionGenerator(canvasTransactionGenerator),
            _currentPlayer(std::make_shared<Player>())
        {
            _currentPlayer->addSinguity(std::make_shared<Singuity>(Vector2D(150, 150)));
            _currentPlayer->addSinguity(std::make_shared<Singuity>(Vector2D(200, 150)));
            _currentPlayer->selectMobileUnitsInArea(Rectangle(Vector2D(0, 0), Vector2D(200, 200)));
            _currentPlayer->setSelectedMobileUnitsDestination(Vector2D(400, 400));
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
            if (!gameIsDone)
            {
                _canvasTransactionGenerator->tryDrawingTransaction([this](std::shared_ptr<SFMLDrawingCanvas> canvas)
                {
                    canvas->draw("Client connected!");
                    for (auto singuity : _currentPlayer->singuities())
                    {
                        canvas->draw(*(GraphicalSinguity(*singuity).drawable()));
                    }
                });
            }
        }

        const std::shared_ptr<CanvasTransactionGenerator> _canvasTransactionGenerator;
        const std::shared_ptr<Player> _currentPlayer;
    };
}
