#pragma once

#include "CommunicatedSpawnerDestination.h"
#include "CommunicatedVector2D.h"

#include <variant>

namespace uw
{
    class CommunicatedSinguityDestination
    {
    public:
        CommunicatedSinguityDestination(std::variant<CommunicatedVector2D, CommunicatedSpawnerDestination> destination) :
            _destination(destination)
        {}

        std::string toJson() const;

        static CommunicatedSinguityDestination fromJson(const std::string& jsonData);

        std::variant<CommunicatedVector2D, CommunicatedSpawnerDestination> destination() const
        {
            return _destination;
        }

    private:
        std::variant<CommunicatedVector2D, CommunicatedSpawnerDestination> _destination;
    };
}