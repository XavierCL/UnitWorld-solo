#include "CommunicationHandler.h"

using namespace uw;

CommunicationHandler::CommunicationHandler(const std::shared_ptr<asio::ip::tcp::socket>& socket, const asio::ip::tcp::endpoint& endpoint) :
    _socket(socket),
    _endpoint(endpoint)
{
}

void CommunicationHandler::send(const std::string& message)
{
    _socket->send(asio::buffer(message));
}

std::string CommunicationHandler::receive()
{
    std::string receivedData;
    receivedData.reserve(10000);
    _socket->receive(asio::buffer(receivedData));
    return receivedData;
}

void CommunicationHandler::close()
{
    _socket->close();
}

std::string CommunicationHandler::prettyName() const
{
    return _endpoint.address().to_string() + ":" + std::to_string(_endpoint.port());
}