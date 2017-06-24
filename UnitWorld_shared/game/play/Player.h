#pragma once

#include "MobileUnit.h"

#include "../geometry/Rectangle.h"

#include "../../utils/Set.hpp"

namespace uw
{
	class Player
	{
	public:
		void selectMobileUnitsInArea(const Rectangle& area);
		void setSelectedMobileUnitsDestination(const Vector2& destination);

	private:
		
		Set<MobileUnit*> _mobileUnits;
		Set<MobileUnit*> _selectedMobileUnits;
	};
}