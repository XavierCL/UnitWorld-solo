#include "ClientGame.h"

#include "clientgui/graphics/canvas/SFMLDrawingCanvas.h"

#include "shared/communication/MessageWrapper.h"
#include "shared/communication/CompleteGameStateMessage.h"

#include "SFML/Graphics.hpp"

#include "immer/vector.hpp"

using namespace uw;

ClientGame::ClientGame(const int& graphicsFramePerSecond, const int& physicsFramePerSecond, std::shared_ptr<Player> currentPlayer, std::shared_ptr<CommunicationHandler> serverHandler, std::shared_ptr<CanvasTransactionGenerator> canvasTransactionGenerator):
    _gameManager(std::make_shared<GameManager>(physicsFramePerSecond)),
    _serverHandler(serverHandler),
    _currentPlayer(currentPlayer),
    _gameDrawer(graphicsFramePerSecond, _gameManager, canvasTransactionGenerator)
{}

ClientGame::~ClientGame()
{
    _serverHandler->close();
    _gameDrawer.stop();
    _gameManager->stop();
    _serverReceiver->join();
}

void ClientGame::startSync()
{
    receiveServerStates();
    std::thread gameDrawerThread([this] { _gameDrawer.startSync(); });
    std::thread gameManagerThread([this] { _gameManager->startSync(); });

    gameDrawerThread.join();
    gameManagerThread.join();
}

// This should be hold in a separate class
void ClientGame::receiveServerStates()
{
    _serverReceiver = std::make_unique<std::thread>([this] {
        while (_serverHandler->isOpen())
        {
            const auto communication = _serverHandler->receive();
            const auto completeStateMessage = std::dynamic_pointer_cast<CompleteGameStateMessage const>(MessageWrapper(communication).innerMessage());

            std::unordered_map<xg::Guid, std::shared_ptr<Player>> completeStatePlayers;
            for (const auto& communicatedPlayer : completeStateMessage->getPlayers())
            {
                completeStatePlayers[communicatedPlayer.playerId()] = std::make_shared<Player>(communicatedPlayer.playerId(), std::vector<std::shared_ptr<Singuity>>());
            }

            for (const auto& communicatedSinguity : completeStateMessage->getSinguities())
            {
                const auto foundPlayer(completeStatePlayers.find(communicatedSinguity.playerId()));

                if (foundPlayer != completeStatePlayers.end())
                {
                    const Vector2D singuityPosition(communicatedSinguity.position().x(), communicatedSinguity.position().y());
                    const Vector2D singuitySpeed(communicatedSinguity.speed().x(), communicatedSinguity.speed().y());
                    const Option<Vector2D> singuityDestination(communicatedSinguity.destination().map<Vector2D>([](const CommunicatedVector2D& actualDestination) {
                        return Vector2D(actualDestination.x(), actualDestination.y());
                    }));

                    foundPlayer->second->addSinguity(std::make_shared<Singuity>(singuityPosition, singuitySpeed, singuityDestination));
                }
            }

            immer::vector<std::shared_ptr<Player>> nextPlayers;
            
            for (const auto& playerIdAndPlayer : completeStatePlayers)
            {
                nextPlayers = std::move(nextPlayers).push_back(playerIdAndPlayer.second);
            }

            _gameManager->setNextPlayers(nextPlayers);
        }
    });
}