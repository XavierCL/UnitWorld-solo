#pragma once

#include "communications/CommunicationHandler.h"

#include "game/GameManager.h"

#include <immer/vector.hpp>

namespace uw
{
    class ServerGame {
    public:

        ServerGame(const unsigned int& physicsFPS, const unsigned int& networkFPS);

        ~ServerGame();

        void addClient(std::shared_ptr<CommunicationHandler> communicationHandler);

    private:

        void sendCompleteState();

        void waitClientReceive();

        void handleClientReceive(std::shared_ptr<CommunicationHandler> callingClient);

        immer::vector<std::shared_ptr<CommunicationHandler>> _communicationHandlers;

        GameManager _gameManager;
        const unsigned int _networkFPS;
        bool _isNetworkRunning;

        std::unique_ptr<std::thread> _gameManagerThread;
        std::unique_ptr<std::thread> _stateSenderThread;
    };
}