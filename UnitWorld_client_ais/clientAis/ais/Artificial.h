#pragma once

#include "clientShared/networking/ServerCommander.h"

#include "shared/game/GameManager.h"

#include <memory>

namespace uw
{
    class Artificial
    {
    public:
        virtual void frameHappened(std::shared_ptr<GameManager> gameManager, std::shared_ptr<ServerCommander> serverCommander) = 0;
    };
}