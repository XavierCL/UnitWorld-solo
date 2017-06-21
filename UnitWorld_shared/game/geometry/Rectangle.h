#pragma once

#include "Point.h"

namespace uw
{
	class Rectangle
	{
	public:
		const bool contains(const Point& point) const
		{
			return point.x >= corner.x && point.x <= corner.x + sizeX && point.y >= corner.y && point.y <= corner.y + sizeY;
		}

		Point corner;
		int sizeX;
		int sizeY;
	};
}