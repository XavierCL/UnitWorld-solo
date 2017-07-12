#pragma once

#include "Singuity.h"

#include "../geometry/Rectangle.h"

#include "../../utils/Set.hpp"

namespace uw
{
	class Player
	{
	public:
		~Player();

		void actualize();
		void addSinguity(Singuity* newSinguity);
		void selectMobileUnitsInArea(const Rectangle& area);
		void setSelectedMobileUnitsDestination(const Vector2D& destination);
		Set<Singuity*> singuities() const;

	private:
		Set<Singuity*> _singuities;
		Set<MobileUnit*> _mobileUnits;
		Set<MobileUnit*> _selectedMobileUnits;
	};
}