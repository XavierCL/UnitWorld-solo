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
    for (auto singuity : _singuities)
    {
        singuity->actualize();
    }
}

void Player::addSinguity(std::shared_ptr<Singuity> newSinguity)
{
    _singuities.insert(newSinguity);
}

void Player::setSinguitiesDestination(const std::unordered_set<xg::Guid>& singuitiesId, const Vector2D& destination)
{
    for (auto singuity : _singuities)
    {
        auto foundSinguity(singuitiesId.find(singuity->id()));
        if (foundSinguity != singuitiesId.cend())
        {
            singuity->setDestination(destination);
        }
    }
}

std::unordered_set<std::shared_ptr<Singuity>, Unit::SharedUnitHash, Unit::SharedUnitEqual> Player::singuities() const
{
    return _singuities;
}