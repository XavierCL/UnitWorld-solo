#pragma once

#include "../geometry/Rectangle.h"
#include "MobileUnit.h"
#include "UnitGenerator.h"
#include "../../utils/Set.hpp"

namespace uw
{
	class Player
	{
	public:
		void moveArea(const Rectangle& area, const Point& destination)
		{
			Set<MobileUnit*> movedUnits;
			for(const auto& mobileUnit: _mobileUnits)
			{
				if(area.contains(mobileUnit.position))
				{
					movedUnits.emplace(&mobileUnit);
				}
			}
			moveUnits(movedUnits, destination);
		}
	private:
		void moveUnits(Set<MobileUnit*>& movedUnits, const Point& destination)
		{
			for(MobileUnit* mobileUnit: movedUnits)
			{
				mobileUnit->move(destination);
			}
		}

		Set<MobileUnit> _mobileUnits;
		Set<UnitGenerator> _generators;
	};
}