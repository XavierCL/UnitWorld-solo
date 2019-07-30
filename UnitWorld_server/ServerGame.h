#pragma once

#include "communications/CommunicationHandler.h"

#include "shared/game/GameManager.h"
#include "shared/communication/MessageSerializer.h"

#include "commons/Guid.hpp"

#include <immer/vector.hpp>

namespace uw
{
    class ServerGame {
    public:

        ServerGame(const unsigned int& physicsFPS, const unsigned int& networkFPS);

        ~ServerGame();

        void addClient(std::shared_ptr<CommunicationHandler> communicationHandler);

    private:

        void loopSendCompleteState();

        void waitClientReceive(const xg::Guid& playerGuid, std::shared_ptr<CommunicationHandler> clientToReceive);

        void handleClientReceive(const xg::Guid& playerGuid, const std::string& receivedCommunication);

        std::vector<std::thread> _clientWaiters;
        immer::vector<std::pair<xg::Guid, std::shared_ptr<CommunicationHandler>>> _communicationHandlers;

        GameManager _gameManager;
        const unsigned int _networkMsPerFrame;
        bool _isNetworkRunning;
        MessageSerializer _messageSerializer;

        std::unique_ptr<std::thread> _gameManagerThread;
        std::unique_ptr<std::thread> _stateSenderThread;
    };
}