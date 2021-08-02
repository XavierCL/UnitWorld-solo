#include "CommunicatedVector2D.h"

#include <nlohmann/json.hpp>

using namespace uw;

CommunicatedVector2D::CommunicatedVector2D(const double& xPosition, const double& yPosition) :
    _x(xPosition),
    _y(yPosition)
{}

nlohmann::json CommunicatedVector2D::toJson() const
{
    nlohmann::json jsonData = {
        {X_LABEL, _x},
        {Y_LABEL, _y}
    };

    return jsonData;
}

CommunicatedVector2D CommunicatedVector2D::fromJson(const nlohmann::json& parsedJson)
{
    return CommunicatedVector2D(parsedJson.at(X_LABEL).get<double>(), parsedJson.at(Y_LABEL).get<double>());
}

double CommunicatedVector2D::x() const
{
    return _x;
}

double CommunicatedVector2D::y() const
{
    return _y;
}

const std::string CommunicatedVector2D::X_LABEL = "x";
const std::string CommunicatedVector2D::Y_LABEL = "y";