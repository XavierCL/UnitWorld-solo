#pragma once

#include <string>

namespace uw
{
    class CommunicatedVector2D
    {
    public:
        CommunicatedVector2D(const double& xPosition, const double& yPosition);

        std::string toJson() const;

        static CommunicatedVector2D fromJson(const std::string& jsonData);

        double x() const;
        double y() const;

    private:

        const double _x;
        const double _y;
    };
}