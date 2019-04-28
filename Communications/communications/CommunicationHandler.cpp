#include "CommunicationHandler.h"

using namespace uw;

CommunicationHandler::CommunicationHandler(const std::shared_ptr<asio::ip::tcp::socket>& socket, const asio::ip::tcp::endpoint& endpoint) :
    _socket(socket),
    _endpoint(endpoint)
{
}

void CommunicationHandler::send()
{
    throw std::exception("Not implemented");
}

void CommunicationHandler::receive()
{
    throw std::exception("Not implemented");
}

std::string CommunicationHandler::prettyName() const
{
    return _endpoint.address().to_string() + ":" + std::to_string(_endpoint.port());
}