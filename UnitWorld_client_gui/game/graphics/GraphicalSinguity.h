#pragma once

#include "GraphicalUnit.h"
#include "game/play/Singuity.h"

namespace uw
{
	class GraphicalSinguity: public GraphicalUnit, public Singuity
	{
	public:
		GraphicalSinguity(Singuity& self):
			Singuity(self)
		{
			auto circleShape = new sf::CircleShape(4);
			circleShape->setPosition(position().x(), position().y());
			circleShape->setFillColor(sf::Color(0, 0, 255));
			_drawable = circleShape;
		}
	};
}