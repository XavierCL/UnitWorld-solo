#include "Player.h"

using namespace uw;

Player::Player(const xg::Guid& id, const std::vector<std::shared_ptr<Singuity>>& singuities):
    _id(id)
{
    for (auto singuity : singuities)
    {
        addSinguity(std::make_shared<Singuity>(*singuity));
    }
}

Player::Player(const Player& other):
    _id(other._id)
{
    for (auto singuity : other._singuities)
    {
        addSinguity(std::make_shared<Singuity>(*singuity));
    }
}

xg::Guid Player::id() const
{
    return _id;
}

void Player::actualize()
{
    for (auto mobileUnit : _mobileUnits)
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
    for (auto const mobileUnit : _mobileUnits)
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

std::unordered_set<std::shared_ptr<const Singuity>, SharedPointerHash<const Singuity>> uw::Player::singuities() const
{
    return _singuities;
}