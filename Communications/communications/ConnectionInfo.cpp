#include "ConnectionInfo.h"

using namespace uw;

ConnectionInfo::ConnectionInfo(const std::string& host, const std::string& port) :
    _host(host),
    _port(port)
{}

std::string ConnectionInfo::host() const
{
    return _host;
}

std::string ConnectionInfo::port() const
{
    return _port;
}