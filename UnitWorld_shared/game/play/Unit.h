#pragma once

#include "../physics/Vector2.h"

namespace uw
{
	class Unit
	{
	public:
		virtual void actualize() = 0;
		Vector2& position();
		void position(const Vector2& newPosition);

	protected:
		Unit(const Vector2& initialPosition);

	private:
		Vector2 _position;
	};
}