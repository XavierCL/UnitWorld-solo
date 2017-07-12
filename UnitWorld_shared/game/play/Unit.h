#pragma once

#include "../physics/Vector2D.h"

namespace uw
{
	class Unit
	{
	public:
		virtual void actualize() = 0;
		Vector2D& position();
		void position(const Vector2D& newPosition);

	protected:
		Unit(const Vector2D& initialPosition);

	private:
		Vector2D _position;
	};
}