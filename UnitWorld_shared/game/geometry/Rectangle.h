#pragma once

#include "Point.h"

namespace uw
{
	class Rectangle
	{
	public:
		Rectangle(const Point& upperLeftCorner = Point(0, 0),
			const unsigned int& width = 0,
			const unsigned int& height = 0);

		const bool contains(const Point& point) const;

	private:
		Point _upperLeftCorner;
		unsigned int _width;
		unsigned int _height;
	};
}