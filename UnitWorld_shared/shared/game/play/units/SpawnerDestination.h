#pragma once

#include "shared/game/play/spawners/SpawnerAllegance.h"

#include "commons/Guid.hpp"
#include "commons/Option.hpp"

namespace uw
{
    class SpawnerDestination
    {
    public:
        SpawnerDestination(const xg::Guid& spawnerId, const Option<SpawnerAllegence>& spawnerAllegence) :
            _spawnerId(spawnerId),
            _spawnerAllegence(spawnerAllegence)
        {}

        const xg::Guid& spawnerId() const &
        {
            return _spawnerId;
        }

        const Option<SpawnerAllegence>& spawnerAllegence() const &
        {
            return _spawnerAllegence;
        }

    private:

        xg::Guid _spawnerId;
        Option<SpawnerAllegence> _spawnerAllegence;
    };
}