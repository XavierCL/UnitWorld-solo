#pragma once

#include <asio.hpp>

namespace uw
{
    class CommunicationHandler
    {
    public:
        CommunicationHandler(const std::shared_ptr<asio::ip::tcp::socket>& socket, const asio::ip::tcp::endpoint& endpoint);

        void send(const std::string& message);

        std::string receive();

        void close();

        bool isOpen() const;

        std::string prettyName() const;

    private:

        static const size_t MAX_TCP_BUFFER_SIZE;

        void resetBufferUntilLastNonZero();

        bool _forceblyClosed;
        std::shared_ptr<asio::ip::tcp::socket> _socket;
        const asio::ip::tcp::endpoint _endpoint;
        std::basic_string<char> _receiveBuffer;
    };
}