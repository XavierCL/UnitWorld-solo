#pragma once

#include "Message.h"

#include "game/CommunicatedVector2D.h"

#include <commons/Guid.hpp>

#include <vector>

namespace uw
{
    class MoveMobileUnitsToPositionMessage: public Message
    {
    public:

        MoveMobileUnitsToPositionMessage(const std::vector<xg::Guid>& singuityIds, const CommunicatedVector2D& destination);

        MoveMobileUnitsToPositionMessage(const std::string& json);

        MessageType messageType() const;

        std::string toJsonData() const;

        std::vector<xg::Guid> singuityIds() const;

        CommunicatedVector2D destination() const;

    private:

        static std::vector<xg::Guid> jsonToSinguityIds(const std::string& json);

        static CommunicatedVector2D jsonToDestination(const std::string& json);

        const std::vector<xg::Guid> _singuityIds;
        const CommunicatedVector2D _destination;
    };
}