#include "Player.h"

#include "commons/CollectionPipe.h"

using namespace uw;

Player::Player(const xg::Guid& id, const std::vector<std::shared_ptr<Singuity>>& singuities):
    _id(id),
    _singuities(std::make_shared<std::vector<std::shared_ptr<Singuity>>>())
{
    for (auto singuity : singuities)
    {
        _singuities->push_back(std::make_shared<Singuity>(*singuity));
    }
}

Player::Player(const Player& other):
    _id(other._id),
    _singuities(std::make_shared<std::vector<std::shared_ptr<Singuity>>>())
{
    for (auto singuity : *other._singuities)
    {
        _singuities->push_back(std::make_shared<Singuity>(*singuity));
    }
}

xg::Guid Player::id() const
{
    return _id;
}

void Player::setSinguitiesDestination(const std::unordered_set<xg::Guid>& singuitiesId, const Vector2D& destination)
{
    _singuities
        | filter<std::shared_ptr<Singuity>>([&singuitiesId](auto singuity) { return singuitiesId.find(singuity->id()) != singuitiesId.cend(); })
        | forEach([&destination](auto singuity) { singuity->setDestination(destination); });
}

std::shared_ptr<std::vector<std::shared_ptr<Singuity>>> Player::singuities() const
{
    return _singuities;
}