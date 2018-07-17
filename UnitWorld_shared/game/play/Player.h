#pragma once

#include "Singuity.h"

#include "../geometry/Rectangle.h"

#include "../../utils/Set.hpp"

#include <memory>

namespace uw
{
	class Player
	{
	public:
		void actualize();
		void addSinguity(std::shared_ptr<Singuity> newSinguity);
		void selectMobileUnitsInArea(const Rectangle& area);
		void setSelectedMobileUnitsDestination(const Vector2D& destination);
		Set<std::shared_ptr<Singuity>> singuities() const;

	private:
		Set<std::shared_ptr<Singuity>> _singuities;
		Set<std::shared_ptr<MobileUnit>> _mobileUnits;
		Set<std::shared_ptr<MobileUnit>> _selectedMobileUnits;
	};
}