#pragma once

#include <nlohmann/json.hpp>

#include <string>

namespace uw
{
    class CommunicatedVector2D
    {
    public:
        CommunicatedVector2D(const double& xPosition, const double& yPosition);

        nlohmann::json toJson() const;

        static CommunicatedVector2D fromJson(const nlohmann::json& jsonData);

        double x() const;
        double y() const;

    private:

        const double _x;
        const double _y;
    };
}