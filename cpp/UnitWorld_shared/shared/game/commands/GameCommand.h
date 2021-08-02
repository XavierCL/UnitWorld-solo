#pragma once

#include "shared/game/play/CompleteGameState.h"

#include <memory>

namespace uw
{
    class GameCommand
    {
    public:
        virtual void execute(std::shared_ptr<CompleteGameState> completeGameState) const = 0;
    };
}