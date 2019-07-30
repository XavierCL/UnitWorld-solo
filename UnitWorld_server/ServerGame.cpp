#include "ServerGame.h"

#include "shared/communication/CompleteGameStateMessage.h"
#include "shared/communication/MessageWrapper.h"

#include "shared/communication/game/CommunicatedPlayer.h"

#include "shared/transfers/PhysicsCommunicationAssembler.h"

#include "commons/Logger.hpp"

#include <chrono>

using namespace uw;

ServerGame::ServerGame(const unsigned int& physicsFPS, const unsigned int& networkFPS) :
    _gameManager(physicsFPS),
    _networkMsPerFrame(1000.0 / networkFPS)
{
    _gameManagerThread = std::make_unique<std::thread>([this] { _gameManager.startSync(); });

    _isNetworkRunning = true;
    _stateSenderThread = std::make_unique<std::thread>([this] { loopSendCompleteState(); });
}

ServerGame::~ServerGame()
{
    _isNetworkRunning = false;
    _gameManager.stop();

    _gameManagerThread->join();
    _stateSenderThread->join();

    for (auto& clientHandler : _communicationHandlers)
    {
        clientHandler.second->close();
    }

    for (auto& clientWaiter : _clientWaiters)
    {
        clientWaiter.join();
    }
}

void ServerGame::addClient(std::shared_ptr<CommunicationHandler> communicationHandler)
{
    const auto singuityInitialXPosition = (double)_communicationHandlers.size();
    auto newPlayer(std::make_shared<Player>(xg::newGuid(), std::vector<std::shared_ptr<Singuity>> {std::make_shared<Singuity>(Vector2D(singuityInitialXPosition, 0), Vector2D(), Option<Vector2D>())}));

    _communicationHandlers = _communicationHandlers.push_back(std::pair(newPlayer->id(), communicationHandler));

    _gameManager.setNextPlayer(newPlayer);
    _clientWaiters.emplace_back([this, newPlayer, communicationHandler] { waitClientReceive(newPlayer->id(), communicationHandler); });
}

void ServerGame::loopSendCompleteState()
{
    PhysicsCommunicationAssembler physicsCommunicationAssembler;

    while (_isNetworkRunning) {
        const auto startFrameTime = std::chrono::steady_clock::now();

        auto localClientCommunicationHandlers(_communicationHandlers);

        const auto players = _gameManager.players();

        std::vector<CommunicatedPlayer> communicatedPlayers;
        std::vector<CommunicatedSinguity> communicatedSinguities;
        for( const auto player : players)
        {
            communicatedPlayers.emplace_back(physicsCommunicationAssembler.physicsPlayerToCommunicated(player));
            const auto assembledPlayerSinguities(physicsCommunicationAssembler.physicsPlayerToCommunicatedSinguities(player));

            std::copy(assembledPlayerSinguities.begin(), assembledPlayerSinguities.end(), std::back_inserter(communicatedSinguities));
        }

        for (auto clientPlayerIdAndCommunicationHandler : localClientCommunicationHandlers)
        {
            const auto message = MessageWrapper(std::make_shared<CompleteGameStateMessage>(communicatedPlayers, communicatedSinguities, clientPlayerIdAndCommunicationHandler.first)).json();
            clientPlayerIdAndCommunicationHandler.second->send(message);
        }

        const auto endFrameTime = std::chrono::steady_clock::now();

        const auto frameTimeInMs = (unsigned int)std::chrono::duration<double, std::milli>(endFrameTime - startFrameTime).count();

        if (frameTimeInMs < _networkMsPerFrame)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(_networkMsPerFrame - frameTimeInMs));
        }
    }
}

void ServerGame::waitClientReceive(const xg::Guid& playerGuid, std::shared_ptr<CommunicationHandler> communicationHandler)
{
    while (_isNetworkRunning)
    {
        std::string receivedCommunication(communicationHandler->receive());

        handleClientReceive(playerGuid, receivedCommunication);
    }
}

void ServerGame::handleClientReceive(const xg::Guid& playerGuid, const std::string& receivedCommunication)
{
    MessageWrapper messageWrapper(receivedCommunication);
    // _gameManager.command(messageWrapper.innerMessage); // or rather a conversion of this particular message
}