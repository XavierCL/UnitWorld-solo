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

        static std::shared_ptr<MoveMobileUnitsToPositionMessage> fromJson(const std::string& json);

        MessageType messageType() const;

        std::string toJsonData() const;

        std::vector<xg::Guid> singuityIds() const;

        CommunicatedVector2D destination() const;

    private:

        const static std::string MoveMobileUnitsToPositionMessage::MOBILE_UNITS_ID_LABEL;
        const static std::string MoveMobileUnitsToPositionMessage::DESTINATION_POSITION_LABEL;

        const std::vector<xg::Guid> _singuityIds;
        const CommunicatedVector2D _destination;
    };
}