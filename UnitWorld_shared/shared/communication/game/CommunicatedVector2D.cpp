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

CommunicatedVector2D CommunicatedVector2D::fromJson(const std::string& jsonData)
{
    nlohmann::json parsedJson = nlohmann::json::parse(jsonData);

    return CommunicatedVector2D(parsedJson.at("x").get<double>(), parsedJson.at("y").get<double>());
}

double CommunicatedVector2D::x() const
{
    return _x;
}

double CommunicatedVector2D::y() const
{
    return _y;
}