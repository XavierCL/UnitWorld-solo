#pragma once

#include <string>

class ConnectionInfo
{
public:
    ConnectionInfo(const std::string& host, const std::string& port) :
        _host(host),
        _port(port)
    {}

    std::string host() const
    {
        return _host;
    }

    std::string port() const
    {
        return _port;
    }

private:
    const std::string _host;
    const std::string _port;
};
