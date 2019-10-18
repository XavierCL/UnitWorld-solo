#pragma once

#include "shared/communication/messages/Message.h"

#include "commons/Guid.hpp"

#include <vector>

namespace uw
{
    class MoveMobileUnitsToSpawnerMessage: public Message
    {
    public:
        MoveMobileUnitsToSpawnerMessage(const std::vector<xg::Guid>& singuityIds, const xg::Guid& spawnerId);

        static std::shared_ptr<MoveMobileUnitsToSpawnerMessage> fromJson(const std::string& json);

        MessageType messageType() const;

        std::string toJsonData() const;

        std::vector<xg::Guid> singuityIds() const;

        xg::Guid spawnerId() const;

    private:

        const static std::string MOBILE_UNITS_ID_LABEL;
        const static std::string SPAWNER_ID_LABEL;

        const std::vector<xg::Guid> _singuityIds;
        const xg::Guid _spawnerId;
    };
}