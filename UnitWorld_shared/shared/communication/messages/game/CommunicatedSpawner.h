#pragma once

#include "CommunicatedSpawnerAllegence.h"
#include "CommunicatedVector2D.h"

#include "commons/Option.hpp"

namespace uw
{
    class CommunicatedSpawner
    {
    public:
        CommunicatedSpawner(const xg::Guid& id, const CommunicatedVector2D& position, const Option<CommunicatedSpawnerAllegence>& allegence, const unsigned long long& lastSpawnTimestamp, const unsigned long long totalSpawnedCount) :
            _id(id),
            _position(position),
            _allegence(allegence),
            _lastSpawnTimestamp(lastSpawnTimestamp),
            _totalSpawnedCount(totalSpawnedCount)
        {}

        xg::Guid id() const
        {
            return _id;
        }

        CommunicatedVector2D position() const
        {
            return _position;
        }

        Option<CommunicatedSpawnerAllegence> allegence() const
        {
            return _allegence;
        }

        unsigned long long lastSpawnTimestamp() const
        {
            return _lastSpawnTimestamp;
        }

        unsigned long long totalSpawnedCount() const
        {
            return _totalSpawnedCount;
        }

        nlohmann::json toJson() const;

        static CommunicatedSpawner fromJson(const nlohmann::json& jsonData);

    private:

        const static std::string NO_ALLEGENCE_VALUE;
        const static std::string SPAWNER_ID_LABEL;
        const static std::string POSITION_LABEL;
        const static std::string ALLEGENCE_LABEL;
        const static std::string LAST_SPAWN_TIMESTAMP_LABEL;
        const static std::string TOTAL_SPAWNED_COUNT_LABEL;

        const xg::Guid _id;
        const CommunicatedVector2D _position;
        const Option<CommunicatedSpawnerAllegence> _allegence;
        const unsigned long long _lastSpawnTimestamp;
        const unsigned long long _totalSpawnedCount;
    };
}