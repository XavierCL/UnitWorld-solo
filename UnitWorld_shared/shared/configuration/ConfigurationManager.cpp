#include "ConfigurationManager.h"

#include <nlohmann/json.hpp>

#include <sstream>
#include <fstream>

using namespace uw;

ConfigurationManager::ConfigurationManager(const std::string& fileName):
    _serverIp(Options::None<std::string>()),
    _serverPort(Options::None<std::string>()),
    _firstSpawners()
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

    if (jsonConfiguration.contains("spawners"))
    {
        for (const auto& jsonSpawner : jsonConfiguration.at("spawners"))
        {
            _firstSpawners.emplace_back(jsonSpawner.at("x").get<double>(), jsonSpawner.at("y").get<double>());
        }
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

std::vector<Vector2D> ConfigurationManager::firstSpawners() const
{
    return _firstSpawners;
}