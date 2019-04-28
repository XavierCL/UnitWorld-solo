#include "GameLoop.h"

#include "game/play/Singuity.h"

#include "../graphics/units/GraphicalSinguity.h"

#include "../graphics/canvas/SFMLDrawingCanvas.h"

#include "SFML/Graphics.hpp"

using namespace uw;

GameLoop::GameLoop(std::shared_ptr<CanvasTransactionGenerator> canvasTransactionGenerator) :
    _canvasTransactionGenerator(canvasTransactionGenerator),
    _currentPlayer(std::make_shared<Player>())
{
    _currentPlayer->addSinguity(std::make_shared<Singuity>(Vector2D(150, 150)));
    _currentPlayer->addSinguity(std::make_shared<Singuity>(Vector2D(200, 150)));
    _currentPlayer->selectMobileUnitsInArea(Rectangle(Vector2D(0, 0), Vector2D(200, 200)));
    _currentPlayer->setSelectedMobileUnitsDestination(Vector2D(400, 400));
}

void GameLoop::loop()
{
    handlePhysics();
    handleGraphics();
}

void GameLoop::handlePhysics()
{
    _currentPlayer->actualize();
}

void GameLoop::handleGraphics()
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