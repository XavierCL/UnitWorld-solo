#pragma once

#include "Unit.h"

namespace uw
{
	class MobileUnit: public Unit
	{
	public:
		MobileUnit();
		~MobileUnit();

		void setDestination(const Point& destination);
	private:
		Point* _destination;
	};
}