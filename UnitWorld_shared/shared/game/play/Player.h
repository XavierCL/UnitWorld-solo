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
        void selectMobileUnitsInArea(const Rectangle& area);
        void setSelectedMobileUnitsDestination(const Vector2D& destination);
        std::unordered_set<std::shared_ptr<const Singuity>, SharedPointerHash<const Singuity>> singuities() const;

    private:
        const xg::Guid _id;

        std::unordered_set<std::shared_ptr<const Singuity>, SharedPointerHash<const Singuity>> _singuities;
        std::unordered_set<std::shared_ptr<MobileUnit>, SharedPointerHash<MobileUnit>> _mobileUnits;
        std::unordered_set<std::shared_ptr<MobileUnit>, SharedPointerHash<MobileUnit>> _selectedMobileUnits;
    };
}