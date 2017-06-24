#pragma once

#include "MobileUnit.h"

#include "../geometry/Rectangle.h"

#include "../../utils/Set.hpp"

namespace uw
{
	class Player
	{
	public:
		void setMobileUnitsDestinationInArea(const Rectangle& area, const Point& destination);

	private:
		static void setMobileUnitsDestination(Set<MobileUnit*>& movedUnits, const Point& destination);
		
		Set<MobileUnit*> _mobileUnits;
	};
}