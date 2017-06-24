#include "Player.h"

using namespace uw;

void Player::setMobileUnitsDestinationInArea(const Rectangle& area, const Point& destination)
{
	Set<MobileUnit*> movedUnits;
	for(const auto* mobileUnit: _mobileUnits)
	{
		if (area.contains(mobileUnit->position()))
		{
			movedUnits.emplace(mobileUnit);
		}
	}
	setMobileUnitsDestination(movedUnits, destination);
}

void Player::setMobileUnitsDestination(Set<MobileUnit*>& mobileUnits, const Point& destination)
{
	for (MobileUnit* mobileUnit : mobileUnits)
	{
		mobileUnit->setDestination(destination);
	}
}