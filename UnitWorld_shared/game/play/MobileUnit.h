#pragma once

#include "Unit.h"

namespace uw
{
	class MobileUnit: public Unit
	{
	public:
		~MobileUnit();

		virtual void actualize();
		void setDestination(const Vector2& destination);
	protected:
		MobileUnit(const MobileUnit& copy);
		MobileUnit(const Vector2& initialPosition);

	private:
		Vector2* _destination;
		Vector2 _speed;
	};
}