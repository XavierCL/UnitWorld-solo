#pragma once

#include <nlohmann/json.hpp>

namespace uw
{
    class CommunicatedVector2D
    {
    public:
        CommunicatedVector2D(const double& xPosition, const double& yPosition):
            _x(xPosition),
            _y(yPosition)
        {}

        std::string toJson() const
        {
            nlohmann::json jsonData = {
                {"x", _x},
                {"y", _y}
            };

            return jsonData;
        }

    private:

        const double _x;
        const double _y;
    };
}