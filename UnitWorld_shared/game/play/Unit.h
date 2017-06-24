#pragma once

#include "../geometry/Point.h"

namespace uw
{
	class Unit
	{
	public:
		Unit(const Point& position = Point(0, 0));

		const Point position() const;
		void position(const Point& newPosition);
	private:
		Point _position;
	};
}