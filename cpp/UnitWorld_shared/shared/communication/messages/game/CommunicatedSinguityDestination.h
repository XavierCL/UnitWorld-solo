#pragma once

#include "CommunicatedSpawnerDestination.h"
#include "CommunicatedVector2D.h"

#include <variant>

namespace uw
{
    class CommunicatedSinguityDestination
    {
    public:
        CommunicatedSinguityDestination(std::variant<CommunicatedVector2D, CommunicatedSpawnerDestination, xg::Guid> destination) :
            _destination(destination)
        {}

        nlohmann::json toJson() const;

        static CommunicatedSinguityDestination fromJson(const nlohmann::json& jsonData);

        std::variant<CommunicatedVector2D, CommunicatedSpawnerDestination, xg::Guid> destination() const
        {
            return _destination;
        }

    private:

        const static std::string POINT_LABEL;
        const static std::string SPAWNER_LABEL;
        const static std::string UNCONDITIONAL_SPAWNER_LABEL;

        std::variant<CommunicatedVector2D, CommunicatedSpawnerDestination, xg::Guid> _destination;
    };
}