#pragma once

#include "../geometry/Point.h"

namespace uw
{
	class Unit
	{
	public:
		const Point position() const;
		void position(const Point& newPosition);
	private:
		Point _position;
	};
}