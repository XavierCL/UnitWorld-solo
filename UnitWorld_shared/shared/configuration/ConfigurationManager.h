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

        std::vector<std::vector<Vector2D>> firstSpawners() const;

        std::vector<std::vector<size_t>> singuitiesBySpawner() const;

        double worldAbsoluteWidth(const double& defaultValue) const;
        double worldAbsoluteHeight(const double& defaultValue) const;
        double sidePanelWidthRatio(const double& defaultValue) const;
        double translationPixelPerFrame(const double& defaultValue) const;
        double scrollRatioPerTick(const double& defaultValue) const;


    private:
        Option<std::string> _serverIp;
        Option<std::string> _serverPort;
        std::vector<std::vector<Vector2D>> _firstSpawners;
        std::vector<std::vector<size_t>> _singuitiesBySpawner;

        Option<double> _worldAbsoluteWidth;
        Option<double> _worldAbsoluteHeight;
        Option<double> _sidePanelWidthRatio;
        Option<double> _translationPixelPerFrame;
        Option<double> _scrollRatioPerTick;
    };
}