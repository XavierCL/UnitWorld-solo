#include "ServerGame.h"

#include "communication/CompleteGameStateMessage.h"
#include "communication/MessageWrapper.h"

#include "communication/game/CommunicatedPlayer.h"

#include "transfers/PhysicsCommunicationAssembler.h"

#include "commons/Logger.hpp"

#include <ctime>

using namespace uw;

ServerGame::ServerGame(const unsigned int& physicsFPS, const unsigned int& networkFPS):
    _gameManager(physicsFPS),
    _networkFPS(networkFPS)
{
    _gameManagerThread = std::make_unique<std::thread>([this] { _gameManager.startSync(); });

    _isNetworkRunning = true;
    _stateSenderThread = std::make_unique<std::thread>([this] { sendCompleteState(); });
}

ServerGame::~ServerGame()
{
    _gameManager.stop();
    _gameManagerThread->join();
}

void ServerGame::addClient(std::shared_ptr<CommunicationHandler> communicationHandler)
{
    _communicationHandlers = _communicationHandlers.push_back(communicationHandler);
}

void ServerGame::sendCompleteState()
{
    PhysicsCommunicationAssembler physicsCommunicationAssembler;

    while (_isNetworkRunning) {
        const auto startFrameTime = clock();

        auto localClientCommunicationHandlers = _communicationHandlers;

        const auto players = _gameManager.threadSafePlayers();
        std::vector<CommunicatedPlayer> communicatedPlayers;
        communicatedPlayers.resize(players.size());
        std::transform(players.begin(), players.end(), communicatedPlayers.begin(), [this](const auto player) { return physicsCommunicationAssembler.physicsPlayerToCommunicated(player);  });
        const auto message = MessageWrapper(std::make_shared<CompleteGameStateMessage>()).json();
        for (auto processingClientCommunicationHandler : localClientCommunicationHandlers)
        {
            processingClientCommunicationHandler->send(message);
        }

        const auto endFrameTime = clock();

        const auto frameTimeInMs = (endFrameTime - startFrameTime) / (CLOCK_PER_SEC / 1000);

        if (frameTimeInMs < _networkFPS)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(_networkFPS - frameTimeInMs));
        }
    }
}

void ServerGame::waitClientReceive()
{}

void ServerGame::handleClientReceive(std::shared_ptr<CommunicationHandler> callingClient)
{}