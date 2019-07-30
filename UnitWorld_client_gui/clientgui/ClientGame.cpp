#include "ClientGame.h"

#include "clientgui/graphics/canvas/SFMLDrawingCanvas.h"

#include "shared/communication/MessageWrapper.h"
#include "shared/communication/CompleteGameStateMessage.h"

#include "SFML/Graphics.hpp"

#include "immer/vector.hpp"

using namespace uw;

ClientGame::ClientGame(const int& graphicsFramePerSecond, const int& physicsFramePerSecond, std::shared_ptr<Player> currentPlayer, std::shared_ptr<CommunicationHandler> serverHandler, std::shared_ptr<CanvasTransactionGenerator> canvasTransactionGenerator, std::shared_ptr<MessageSerializer> messageSerializer):
    _gameManager(std::make_shared<GameManager>(physicsFramePerSecond)),
    _serverHandler(serverHandler),
    _currentPlayer(currentPlayer),
    _gameDrawer(graphicsFramePerSecond, _gameManager, canvasTransactionGenerator),
    _serverReceiver(serverHandler, _gameManager, messageSerializer)
{}

ClientGame::~ClientGame()
{
    stop();
}

void ClientGame::startSync()
{
    std::thread serverReceiverThread([this] { _serverReceiver.startSync(); });
    std::thread gameDrawerThread([this] { _gameDrawer.startSync(); });
    std::thread gameManagerThread([this] { _gameManager->startSync(); });

    serverReceiverThread.join();
    gameDrawerThread.join();
    gameManagerThread.join();
}

void ClientGame::stop()
{
    _serverReceiver.stop();
    _gameDrawer.stop();
    _gameManager->stop();
}