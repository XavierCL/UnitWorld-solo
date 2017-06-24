#pragma once

#include "../physics/Vector2.h"

namespace uw
{
	class Unit
	{
	public:
		Unit(const Vector2& initialPosition);

		const Vector2 position() const;
		void position(const Vector2& newPosition);
	private:
		Vector2 _position;
	};
}