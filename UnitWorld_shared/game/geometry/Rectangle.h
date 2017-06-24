#pragma once

#include "Point.h"

namespace uw
{
	class Rectangle
	{

	public:
		const bool contains(const Point& point) const;

	private:
		Point _upperLeftCorner;
		unsigned int _width;
		unsigned int _height;
	};
}