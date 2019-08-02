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
    try
    {
        asio::write(*_socket, asio::buffer(message));
    }
    catch (...)
    {
        // socket was likely closed
    }
}

std::string CommunicationHandler::receive()
{
    try
    {
        _socket->read_some(asio::buffer(_receiveBuffer));
        const std::string returnedBuffer(_receiveBuffer.c_str());
        resetBufferUntilLastNonZero();
        return returnedBuffer;
    }
    catch (...)
    {
        // socket was likely closed
        return "";
    }
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

void CommunicationHandler::resetBufferUntilLastNonZero()
{
    for (size_t bufferIndex = 0; bufferIndex < _receiveBuffer.size(); ++bufferIndex)
    {
        if (_receiveBuffer[bufferIndex] == '\0') break;

        _receiveBuffer[bufferIndex] = '\0';
    }
}