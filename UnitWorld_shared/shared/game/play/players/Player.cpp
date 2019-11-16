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

void Player::setSinguitiesDestination(const immer::set<xg::Guid>& singuitiesId, const Vector2D& destination)
{
    _singuities
        | filter<std::shared_ptr<Singuity>>([&singuitiesId](auto singuity) { return singuitiesId.count(singuity->id()) > 0; })
        | forEach([&destination](auto singuity) { singuity->setPointDestination(destination); });
}

void Player::setSinguitiesSpawnerDestination(const immer::set<xg::Guid>& singuitiesId, const SpawnerDestination& spawnerDestination)
{
    _singuities
        | filter<std::shared_ptr<Singuity>>([&singuitiesId](std::shared_ptr<Singuity> singuity) { return singuitiesId.count(singuity->id()) > 0; })
        | forEach([&spawnerDestination](std::shared_ptr<Singuity> singuity) { singuity->setSpawnerDestination(spawnerDestination); });
}

void Player::addSinguity(std::shared_ptr<Singuity> newSinguity)
{
    _singuities->emplace_back(newSinguity);
    
    auto addSinguityCallbacks = _singuityAddedCallbacks;
    for (const auto& callback : addSinguityCallbacks)
    {
        callback.second(newSinguity);
    }
}

void Player::setSinguities(std::shared_ptr<std::vector<std::shared_ptr<Singuity>>> singuities)
{
    _singuities = singuities;
}

std::shared_ptr<std::vector<std::shared_ptr<Singuity>>> Player::singuities() const
{
    return _singuities;
}

void Player::addSinguityAddedCallback(const xg::Guid& callbackId, const std::function<void(std::shared_ptr<Singuity>)>& callback)
{
    _singuityAddedCallbacks = _singuityAddedCallbacks.insert(std::make_pair(callbackId, callback));
}

void Player::removeSinguityAddedCallback(const xg::Guid& callbackId)
{
    _singuityAddedCallbacks = _singuityAddedCallbacks.erase(callbackId);
}