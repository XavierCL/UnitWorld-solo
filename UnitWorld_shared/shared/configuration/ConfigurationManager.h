#pragma once

#include <commons/Option.hpp>

#include <string>

namespace uw
{
    class ConfigurationManager
    {
    public:
        ConfigurationManager(const std::string& fileName);

        std::string serverIpOrDefault(const std::string& defaultValue) const;

        std::string serverPortOrDefault(const std::string& defaultValue) const;

    private:
        Option<std::string> _serverIp;
        Option<std::string> _serverPort;
    };
}