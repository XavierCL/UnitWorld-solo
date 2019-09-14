#include "GraphicalSinguity.h"

#include <cmath>

using namespace uw;

GraphicalSinguity::GraphicalSinguity(std::shared_ptr<Singuity> self, const Circle& selfShape, const sf::Color& color)
{
    auto lifeRatio = self->healthPoint() / self->maximumHealthPoint();
    sf::Color lifeColor(round(color.r * lifeRatio), round(color.g * lifeRatio), round(color.b * lifeRatio));

    const auto innerSinguityRadius(selfShape.radius() * 0.75);
    auto circleShape = std::make_shared<sf::CircleShape>(innerSinguityRadius);
    circleShape->setPosition(round(selfShape.center().x() - innerSinguityRadius), round(selfShape.center().y() - innerSinguityRadius));
    circleShape->setFillColor(lifeColor);
    circleShape->setOutlineThickness(selfShape.radius() * 0.25);
    circleShape->setOutlineColor(color);
    _drawable = circleShape;
}