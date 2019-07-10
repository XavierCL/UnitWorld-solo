#pragma once

#include "Message.h"

#include "game/CommunicatedPlayer.h"
#include "game/CommunicatedSinguity.h"

namespace uw
{
    class CompleteGameStateMessage : public Message
    {
    public:
        CompleteGameStateMessage(const std::vector<CommunicatedPlayer>& players, const std::vector<CommunicatedSinguity>& singuities);

        CompleteGameStateMessage(const std::string jsonData);

        virtual MessageType messageType() const;

        virtual std::string toJsonData() const;

    private:

        std::vector<CommunicatedPlayer> _players;
        std::vector<CommunicatedSinguity> _singuities;
    };
}