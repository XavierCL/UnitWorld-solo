#pragma once

#include <asio.hpp>

class CommunicationHandler
{
public:
    CommunicationHandler(const std::shared_ptr<asio::ip::tcp::socket>& socket, const asio::ip::tcp::endpoint& endpoint) :
        _socket(socket),
        _endpoint(endpoint)
    {
    }

    void send()
    {

    }

    void receive()
    {
    }

    std::string prettyName() const
    {
        return _endpoint.address().to_string() + ":" + std::to_string(_endpoint.port());
    }

private:
    std::shared_ptr<const asio::ip::tcp::socket> _socket;
    const asio::ip::tcp::endpoint _endpoint;
};
