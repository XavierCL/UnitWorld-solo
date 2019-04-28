#pragma once

#include "game/play/Player.h"

#include "../graphics/canvas/CanvasTransactionGenerator.h"

namespace uw
{
    class GameLoop
    {
    public:

        GameLoop(std::shared_ptr<CanvasTransactionGenerator> canvasTransactionGenerator);

        void loop();

    private:
        static const bool GAME_STATE_ENDED = true;
        static const bool GAME_STATE_RUNNING = false;

        bool handlePhysics();

        void handleGraphics(const bool& gameIsDone);

        const std::shared_ptr<CanvasTransactionGenerator> _canvasTransactionGenerator;
        const std::shared_ptr<Player> _currentPlayer;
    };
}
