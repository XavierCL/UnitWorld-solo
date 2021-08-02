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
    _singuitiesBySpawner(),
    _worldAbsoluteWidth(),
    _worldAbsoluteHeight(),
    _sidePanelWidthRatio(),
    _translationPixelPerFrame(),
    _scrollRatioPerTick(),
    _aiName(),
    _isFogOfWarEnabled()
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

        if (jsonConfiguration.contains("world-absolute-width"))
        {
            _worldAbsoluteWidth = Options::Some(jsonConfiguration.at("world-absolute-width").get<double>());
        }

        if (jsonConfiguration.contains("world-absolute-height"))
        {
            _worldAbsoluteHeight = Options::Some(jsonConfiguration.at("world-absolute-height").get<double>());
        }

        if (jsonConfiguration.contains("move-zone-width-ratio"))
        {
            _sidePanelWidthRatio = Options::Some(jsonConfiguration.at("move-zone-width-ratio").get<double>());
        }

        if (jsonConfiguration.contains("translation-pixel-per-frame"))
        {
            _translationPixelPerFrame = Options::Some(jsonConfiguration.at("translation-pixel-per-frame").get<double>());
        }

        if (jsonConfiguration.contains("scroll-ratio-per-tick"))
        {
            _scrollRatioPerTick = Options::Some(jsonConfiguration.at("scroll-ratio-per-tick").get<double>());
        }

        if (jsonConfiguration.contains("ai-name"))
        {
            _aiName = Options::Some(jsonConfiguration.at("ai-name").get<std::string>());
        }

        if (jsonConfiguration.contains("fog-of-war"))
        {
            _isFogOfWarEnabled = Options::Some(jsonConfiguration.at("fog-of-war").get<bool>());
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

double ConfigurationManager::worldAbsoluteWidth(const double& defaultValue) const
{
    return _worldAbsoluteWidth.getOrElse(defaultValue);
}

double ConfigurationManager::worldAbsoluteHeight(const double& defaultValue) const
{
    return _worldAbsoluteHeight.getOrElse(defaultValue);
}

double ConfigurationManager::sidePanelWidthRatio(const double& defaultValue) const
{
    return _sidePanelWidthRatio.getOrElse(defaultValue);
}

double ConfigurationManager::translationPixelPerFrame(const double& defaultValue) const
{
    return _translationPixelPerFrame.getOrElse(defaultValue);
}

double ConfigurationManager::scrollRatioPerTick(const double& defaultValue) const
{
    return _scrollRatioPerTick.getOrElse(defaultValue);
}

std::string ConfigurationManager::aiName(const std::string& defaultValue) const
{
    return _aiName.getOrElse(defaultValue);
}

bool ConfigurationManager::isFogOfWarEnabled(const bool& defaultValue) const
{
    return _isFogOfWarEnabled.getOrElse(defaultValue);
}