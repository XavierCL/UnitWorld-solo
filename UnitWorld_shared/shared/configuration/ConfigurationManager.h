#pragma once

#include <commons/Option.hpp>

#include <shared/game/geometry/Vector2D.h>

#include <string>
#include <vector>

namespace uw
{
    class ConfigurationManager
    {
    public:
        ConfigurationManager(const std::string& fileName);

        std::string serverIpOrDefault(const std::string& defaultValue) const;

        std::string serverPortOrDefault(const std::string& defaultValue) const;

        std::vector<Vector2D> firstSpawners() const;

    private:
        Option<std::string> _serverIp;
        Option<std::string> _serverPort;
        std::vector<Vector2D> _firstSpawners;
    };
}