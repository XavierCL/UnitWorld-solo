#include "CommunicationHandler.h"

using namespace uw;

CommunicationHandler::CommunicationHandler(const std::shared_ptr<asio::ip::tcp::socket>& socket, const asio::ip::tcp::endpoint& endpoint) :
    _forceblyClosed(false),
    _socket(socket),
    _endpoint(endpoint)
{
    if (!_socket->is_open()) throw std::invalid_argument("socket was closed at communication handler creation");
    // Max TCP receive size
    _receiveBuffer.resize(65535);
}

void CommunicationHandler::send(const std::string& message)
{
    if (!isOpen()) throw std::logic_error("socket was closed while trying to send data through it: " + message.substr(0, 100) + (message.size() > 100 ? "..." : ""));

    try
    {
        asio::write(*_socket, asio::buffer(message));
    }
    catch (...)
    {
        _forceblyClosed = true;
        throw;
    }
}

std::string CommunicationHandler::receive()
{
    if (!isOpen()) throw std::logic_error("socket was closed while trying to receive from it");

    try
    {
        _socket->read_some(asio::buffer(_receiveBuffer));
        const std::string returnedBuffer(_receiveBuffer.c_str());
        resetBufferUntilLastNonZero();
        return returnedBuffer;
    }
    catch (...)
    {
        _forceblyClosed = true;
        throw;
    }
}

void CommunicationHandler::close()
{
    if (isOpen())
    {
        _socket->close();
    }
}

bool CommunicationHandler::isOpen() const
{
    return !_forceblyClosed && _socket->is_open();
}

std::string CommunicationHandler::prettyName() const
{
    return _endpoint.address().to_string() + ":" + std::to_string(_endpoint.port());
}

void CommunicationHandler::resetBufferUntilLastNonZero()
{
    for (size_t bufferIndex = 0; bufferIndex < _receiveBuffer.size(); ++bufferIndex)
    {
        if (_receiveBuffer[bufferIndex] == '\0') break;

        _receiveBuffer[bufferIndex] = '\0';
    }
}