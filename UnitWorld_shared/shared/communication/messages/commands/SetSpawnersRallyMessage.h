#pragma once

#include "shared/communication/messages/Message.h"

#include "shared/communication/messages/game/CommunicatedSinguityDestination.h"

#include <commons/Guid.hpp>

#include <vector>

namespace uw
{
    class SetSpawnersRallyMessage : public Message
    {
    public:

        SetSpawnersRallyMessage(const std::vector<xg::Guid>& spawnersId, const CommunicatedSinguityDestination& destination);

        static std::shared_ptr<SetSpawnersRallyMessage> fromJson(const std::string& json);

        MessageType messageType() const;

        std::string toJsonData() const;

        std::vector<xg::Guid> spawnersId() const;

        CommunicatedSinguityDestination destination() const;

    private:

        const static std::string SPAWNERS_ID_LABEL;
        const static std::string DESTINATION_LABEL;

        const std::vector<xg::Guid> _spawnersId;
        const CommunicatedSinguityDestination _destination;
    };
}