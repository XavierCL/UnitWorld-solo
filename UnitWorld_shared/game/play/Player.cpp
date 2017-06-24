#include "Player.h"

using namespace uw;

void Player::selectMobileUnitsInArea(const Rectangle& area)
{
	for(auto* const mobileUnit: _mobileUnits)
	{
		if (area.contains(mobileUnit->position()))
		{
			_selectedMobileUnits.insert(mobileUnit);
		}
	}
}

void Player::setSelectedMobileUnitsDestination(const Vector2& destination)
{
	for (MobileUnit* mobileUnit : _selectedMobileUnits)
	{
		mobileUnit->setDestination(destination);
	}
}