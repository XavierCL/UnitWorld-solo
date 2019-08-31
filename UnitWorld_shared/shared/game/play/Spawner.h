#pragma once

#include "SpawnerAllegance.h"

#include "Unit.h"

#include "commons/Option.hpp"
#include "commons/Guid.hpp"

namespace uw
{
    class Spawner: public Unit
    {
    public:
        Spawner(const Vector2D& initialPosition, Option<SpawnerAllegence> allegence) :
            Unit(initialPosition),
            _allegence(allegence)
        {}

        Spawner(const xg::Guid& id, const Vector2D& initialPosition, Option<SpawnerAllegence> allegence) :
            Unit(id, initialPosition),
            _allegence(allegence)
        {}

        Option<SpawnerAllegence> allegence() const
        {
            return _allegence;
        }

        double maxHealthPoint() const
        {
            return 2000.0;
        }

    private:
        const Option<SpawnerAllegence> _allegence;
    };
}