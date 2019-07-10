#include "ServerGame.h"

#include "communication/CompleteGameStateMessage.h"
#include "communication/MessageWrapper.h"

#include "communication/game/CommunicatedPlayer.h"

#include "transfers/PhysicsCommunicationAssembler.h"

#include "commons/Logger.hpp"

#include <ctime>

using namespace uw;

ServerGame::ServerGame(const unsigned int& physicsFPS, const unsigned int& networkFPS) :
    _gameManager(physicsFPS),
    _networkFPS(networkFPS)
{
    _gameManagerThread = std::make_unique<std::thread>([this] { _gameManager.startSync(); });

    _isNetworkRunning = true;
    _stateSenderThread = std::make_unique<std::thread>([this] { loopSendCompleteState(); });
}

ServerGame::~ServerGame()
{
    _gameManager.stop();
    _isNetworkRunning = false;

    _gameManagerThread->join();
    _stateSenderThread->join();

    for (auto& clientHandler : _communicationHandlers)
    {
        clientHandler->close();
    }

    for (auto& clientWaiter : _clientWaiters)
    {
        clientWaiter.join();
    }
}

void ServerGame::addClient(std::shared_ptr<CommunicationHandler> communicationHandler)
{
    _communicationHandlers = _communicationHandlers.push_back(communicationHandler);

    const auto singuityInitialXPosition = (double)_communicationHandlers.size();
    auto newPlayer(std::make_shared<Player>(std::vector<std::shared_ptr<Singuity>> {std::make_shared<Singuity>(singuityInitialXPosition, 0)}));

    _gameManager.addPlayer(newPlayer);
    _clientWaiters.emplace_back([this, newPlayer, communicationHandler] { waitClientReceive(newPlayer->id(), communicationHandler); });
}

void ServerGame::loopSendCompleteState()
{
    PhysicsCommunicationAssembler physicsCommunicationAssembler;

    while (_isNetworkRunning) {
        const auto startFrameTime = clock();

        auto localClientCommunicationHandlers = _communicationHandlers;

        const auto players = _gameManager.threadSafePlayers();

        std::vector<CommunicatedPlayer> communicatedPlayers;
        std::vector<CommunicatedSinguity> communicatedSinguities;
        for( const auto player : players)
        {
            communicatedPlayers.emplace_back(physicsCommunicationAssembler.physicsPlayerToCommunicated(player));
            const auto assembledPlayerSinguities = physicsCommunicationAssembler.physicsPlayerToCommunicatedSinguities(player);
            communicatedSinguities.insert(communicatedSinguities.cend(), assembledPlayerSinguities.begin(), assembledPlayerSinguities.end());
        }

        const auto message = MessageWrapper(std::make_shared<CompleteGameStateMessage>(communicatedPlayers, communicatedSinguities)).json();
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

void ServerGame::waitClientReceive(const xg::Guid& playerGuid, std::shared_ptr<CommunicationHandler> communicationHandler)
{
    while (_isNetworkRunning)
    {
        const auto receivedCommunication = communicationHandler->receive();

        handleClientReceive(playerGuid, receivedCommunication);
    }
}

void ServerGame::handleClientReceive(const xg::Guid& playerGuid, const std::string& receivedCommunication)
{
    MessageWrapper messageWrapper(receivedCommunication);
    // _gameManager.command(messageWrapper.innerMessage); // or rather a conversion of this particular message
}