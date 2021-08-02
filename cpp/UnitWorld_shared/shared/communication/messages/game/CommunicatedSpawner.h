#pragma once

#include "CommunicatedSpawnerAllegence.h"
#include "CommunicatedSinguityDestination.h"
#include "CommunicatedVector2D.h"

#include "commons/Option.hpp"

namespace uw
{
    class CommunicatedSpawner
    {
    public:
        CommunicatedSpawner(const xg::Guid& id, const CommunicatedVector2D& position, const Option<CommunicatedSpawnerAllegence>& allegence, const Option<CommunicatedSinguityDestination>& rally, const long long& lastSpawnFrameCount, const long long totalSpawnedCount, const long long lastClaimedFrameCount) :
            _id(id),
            _position(position),
            _allegence(allegence),
            _rally(rally),
            _lastSpawnFrameCount(lastSpawnFrameCount),
            _totalSpawnedCount(totalSpawnedCount),
            _lastClaimedFrameCount(lastClaimedFrameCount)
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

        Option<CommunicatedSinguityDestination> rally() const
        {
            return _rally;
        }

        long long lastSpawnFrameCount() const
        {
            return _lastSpawnFrameCount;
        }

        long long totalSpawnedCount() const
        {
            return _totalSpawnedCount;
        }

        long long lastClaimedFrameCount() const
        {
            return _lastClaimedFrameCount;
        }

        nlohmann::json toJson() const;

        static CommunicatedSpawner fromJson(const nlohmann::json& jsonData);

    private:

        const static std::string NO_ALLEGENCE_VALUE;
        const static std::string NO_RALLY_VALUE;
        const static std::string SPAWNER_ID_LABEL;
        const static std::string POSITION_LABEL;
        const static std::string ALLEGENCE_LABEL;
        const static std::string RALLY_LABEL;
        const static std::string LAST_SPAWN_FRAME_COUNT_LABEL;
        const static std::string TOTAL_SPAWNED_COUNT_LABEL;
        const static std::string LAST_CLAIMED_FRAME_COUNT_LABEL;

        const xg::Guid _id;
        const CommunicatedVector2D _position;
        const Option<CommunicatedSpawnerAllegence> _allegence;
        const Option<CommunicatedSinguityDestination> _rally;
        const long long _lastSpawnFrameCount;
        const long long _totalSpawnedCount;
        const long long _lastClaimedFrameCount;
    };
}