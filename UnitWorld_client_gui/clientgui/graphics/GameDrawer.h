#pragma once

#include "canvas/SFMLDrawingCanvas.h"
#include "units/GraphicalSinguity.h"

#include "shared/game/GameManager.h"

namespace uw
{
    class GameDrawer
    {
    public:
        GameDrawer(std::shared_ptr<GameManager> gameManager):
            _gameManager(gameManager)
        {}

        void draw(std::shared_ptr<SFMLDrawingCanvas> canvas)
        {
            for (auto singuity : _gameManager->singuities())
            {
                canvas->draw(*(GraphicalSinguity(*singuity).drawable()));
            }
        }

    private:

        const std::shared_ptr<GameManager> _gameManager;
    };
}