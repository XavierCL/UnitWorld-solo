#include "CommunicatedVector2D.h"

#include <nlohmann/json.hpp>

using namespace uw;

CommunicatedVector2D::CommunicatedVector2D(const double& xPosition, const double& yPosition) :
    _x(xPosition),
    _y(yPosition)
{}

std::string CommunicatedVector2D::toJson() const
{
    nlohmann::json jsonData = {
        {"x", _x},
        {"y", _y}
    };

    return jsonData;
}