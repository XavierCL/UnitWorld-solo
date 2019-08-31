#pragma once

#include "commons/Guid.hpp"

namespace uw
{
    class SpawnerAllegence
    {
    public:

        bool isClaimed() const;

        double healthPoint() const;

        xg::Guid allegedPlayerId() const;
    };
}