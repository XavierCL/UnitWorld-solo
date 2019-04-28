#pragma once

#include <string>

namespace uw
{
    class ConnectionInfo
    {
    public:
        ConnectionInfo(const std::string& host, const std::string& port);

        std::string host() const;

        std::string port() const;

    private:
        const std::string _host;
        const std::string _port;
    };
}