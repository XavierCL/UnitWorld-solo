#pragma once

#include "clientAis/ais/Artificial.h"

namespace uw
{
    class XavierclAi1: public Artificial
    {
    public:
        virtual void frameHappened(std::shared_ptr<GameManager> gameManager, std::shared_ptr<ServerCommander> serverCommander)
        {

        }
    };
}