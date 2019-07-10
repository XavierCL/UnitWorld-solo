#pragma once

#include <string>

namespace uw
{
    class CommunicatedVector2D
    {
    public:
        CommunicatedVector2D(const double& xPosition, const double& yPosition);

        std::string toJson() const;

    private:

        const double _x;
        const double _y;
    };
}