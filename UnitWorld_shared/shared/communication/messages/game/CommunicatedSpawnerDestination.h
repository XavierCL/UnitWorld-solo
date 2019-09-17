#pragma once

#include "CommunicatedSpawnerAllegence.h"

#include "commons/Guid.hpp"
#include "commons/Option.hpp"

namespace uw
{
    class CommunicatedSpawnerDestination
    {
    public:
        CommunicatedSpawnerDestination(const xg::Guid& spawnerId, const Option<CommunicatedSpawnerAllegence>& spawnerAllegence) :
            _spawnerId(spawnerId),
            _spawnerAllegence(spawnerAllegence)
        {}

        nlohmann::json toJson() const;

        static CommunicatedSpawnerDestination fromJson(const nlohmann::json& jsonData);

        xg::Guid spawnerId() const
        {
            return _spawnerId;
        }

        Option<CommunicatedSpawnerAllegence> spawnerAllegence() const
        {
            return _spawnerAllegence;
        }

    private:
        xg::Guid _spawnerId;
        Option<CommunicatedSpawnerAllegence> _spawnerAllegence;
    };
}