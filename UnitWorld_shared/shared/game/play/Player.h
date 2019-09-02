#pragma once

#include "Singuity.h"

#include "../geometry/Rectangle.h"

#include "commons/Hash.hpp"
#include "commons/Guid.hpp"

#include <unordered_set>

namespace uw
{
    class Player
    {
    public:
        Player(const xg::Guid& id, const std::vector<std::shared_ptr<Singuity>>& singuities);
        Player(const Player& other);

        xg::Guid id() const;
        void setSinguitiesDestination(const std::unordered_set<xg::Guid>& singuitiesId, const Vector2D& destination);
        void addSinguity(std::shared_ptr<Singuity> newSinguity);
        void setSinguities(std::shared_ptr<std::vector<std::shared_ptr<Singuity>>> singuities);
        std::shared_ptr<std::vector<std::shared_ptr<Singuity>>> singuities() const;

        void OnSinguityAdded(const xg::Guid& callbackId, const std::function<void(std::shared_ptr<Singuity>)>& callback);

    private:
        const xg::Guid _id;

        std::shared_ptr<std::vector<std::shared_ptr<Singuity>>> _singuities;
    };
}