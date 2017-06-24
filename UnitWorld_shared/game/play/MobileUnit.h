#pragma once

#include "Unit.h"

namespace uw
{
	class MobileUnit: public Unit
	{
	public:
		~MobileUnit();

		void setDestination(const Vector2& destination);
	private:
		Vector2* _destination;
	};
}