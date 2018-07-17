#include "Player.h"

using namespace uw;

void Player::actualize()
{
	for(auto mobileUnit: _mobileUnits)
	{
		mobileUnit->actualize();
	}
}

void uw::Player::addSinguity(std::shared_ptr<Singuity> newSinguity)
{
	_singuities.insert(newSinguity);
	_mobileUnits.insert(newSinguity);
}

void Player::selectMobileUnitsInArea(const Rectangle& area)
{
	for(auto const mobileUnit: _mobileUnits)
	{
		if (area.contains(mobileUnit->position()))
		{
			_selectedMobileUnits.insert(mobileUnit);
		}
	}
}

void Player::setSelectedMobileUnitsDestination(const Vector2D& destination)
{
	for (auto mobileUnit : _selectedMobileUnits)
	{
		mobileUnit->setDestination(destination);
	}
}

Set<std::shared_ptr<Singuity>> uw::Player::singuities() const
{
	return _singuities;
}
