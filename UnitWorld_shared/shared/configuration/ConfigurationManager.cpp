#include "ConfigurationManager.h"

#include <nlohmann/json.hpp>

#include <sstream>
#include <fstream>

using namespace uw;

ConfigurationManager::ConfigurationManager(const std::string& fileName):
    _serverIp(Options::None<std::string>()),
    _serverPort(Options::None<std::string>())
{
    std::ifstream configurationFile(fileName);
    const auto jsonConfiguration(nlohmann::json::parse(configurationFile, nullptr, false));

    if (jsonConfiguration.contains("server ip"))
    {
        _serverIp = Options::Some(jsonConfiguration.at("server ip").get<std::string>());
    }

    if (jsonConfiguration.contains("server port"))
    {
        _serverPort = Options::Some(jsonConfiguration.at("server port").get<std::string>());
    }
}

std::string ConfigurationManager::serverIpOrDefault(const std::string& defaultValue) const
{
    return _serverIp.getOrElse(defaultValue);
}

std::string ConfigurationManager::serverPortOrDefault(const std::string& defaultValue) const
{
    return _serverPort.getOrElse(defaultValue);
}