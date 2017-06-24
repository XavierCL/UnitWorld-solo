#pragma once

#include "../physics/Vector2.h"

namespace uw
{
	class Rectangle
	{
	public:
		Rectangle(const Vector2& corner1,
			const Vector2& corner2);

		const bool contains(const Vector2& point) const;

	private:
		Vector2 _upperLeftCorner;
		Vector2 _lowerRightCorner;
	};
}