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

        const static std::string NO_ALLEGENCE_VALUE;
        const static std::string SPAWNER_ALLEGENCE_LABEL;
        const static std::string SPAWNER_ID_LABEL;

        xg::Guid _spawnerId;
        Option<CommunicatedSpawnerAllegence> _spawnerAllegence;
    };
}