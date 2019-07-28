#include "CommunicationHandler.h"

using namespace uw;

CommunicationHandler::CommunicationHandler(const std::shared_ptr<asio::ip::tcp::socket>& socket, const asio::ip::tcp::endpoint& endpoint) :
    _socket(socket),
    _endpoint(endpoint)
{
    // Max TCP receive size
    _receiveBuffer.resize(65535);
}

void CommunicationHandler::send(const std::string& message)
{
    asio::write(*_socket, asio::buffer(message));
}

std::string CommunicationHandler::receive()
{
    _socket->read_some(asio::buffer(_receiveBuffer));
    return _receiveBuffer;
}

void CommunicationHandler::close()
{
    _socket->close();
}

bool CommunicationHandler::isOpen() const
{
    return _socket->is_open();
}

std::string CommunicationHandler::prettyName() const
{
    return _endpoint.address().to_string() + ":" + std::to_string(_endpoint.port());
}