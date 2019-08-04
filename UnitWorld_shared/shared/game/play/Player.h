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
        void actualize();
        void addSinguity(std::shared_ptr<Singuity> newSinguity);
        void setSinguitiesDestination(const std::unordered_set<xg::Guid>& singuitiesId, const Vector2D& destination);
        std::unordered_set<std::shared_ptr<Singuity>, Unit::SharedUnitHash, Unit::SharedUnitEqual> singuities() const;

    private:
        const xg::Guid _id;

        std::unordered_set<std::shared_ptr<Singuity>, Unit::SharedUnitHash, Unit::SharedUnitEqual> _singuities;
    };
}