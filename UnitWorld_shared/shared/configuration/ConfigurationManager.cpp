#include "ConfigurationManager.h"

#include "commons/Logger.hpp"

#include <nlohmann/json.hpp>

#include <sstream>
#include <fstream>

using namespace uw;

ConfigurationManager::ConfigurationManager(const std::string& fileName):
    _serverIp(Options::None<std::string>()),
    _serverPort(Options::None<std::string>()),
    _firstSpawners(),
    _singuitiesBySpawner()
{
    try
    {
        std::ifstream configurationFile(fileName);
        const auto jsonConfiguration(nlohmann::json::parse(configurationFile));

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
            for (const auto& jsonSpawners : jsonConfiguration.at("spawners"))
            {
                _firstSpawners.emplace_back();
                if (_firstSpawners.size() > 1)
                {
                    _singuitiesBySpawner.emplace_back();
                }

                for (const auto& jsonSpawner : jsonSpawners)
                {
                    _firstSpawners.back().emplace_back(jsonSpawner.at("x").get<double>(), jsonSpawner.at("y").get<double>());

                    if (_firstSpawners.size() > 1)
                    {
                        if (jsonSpawner.contains("singuities"))
                        {
                            _singuitiesBySpawner.back().emplace_back(jsonSpawner.at("singuities").get<size_t>());
                        }
                        else
                        {
                            _singuitiesBySpawner.back().emplace_back(0);
                        }
                    }
                }
            }
        }
    }
    catch (nlohmann::json::exception jsonError)
    {
        Logger::error("Json error while parsing the config file, default values will be used. Error: " + std::string(jsonError.what()));
    }
    catch (std::exception error)
    {
        Logger::error("Unknown error while parsing the config file, default values will be used. Error: " + std::string(error.what()));
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

std::vector<std::vector<Vector2D>> ConfigurationManager::firstSpawners() const
{
    return _firstSpawners;
}

std::vector<std::vector<size_t>> ConfigurationManager::singuitiesBySpawner() const
{
    return _singuitiesBySpawner;
}