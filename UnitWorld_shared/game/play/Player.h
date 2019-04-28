#pragma once

#include "Singuity.h"

#include "../geometry/Rectangle.h"

#include "commons/Hash.hpp"

#include <unordered_set>

namespace uw
{
    class Player
    {
    public:
        void actualize();
        void addSinguity(std::shared_ptr<Singuity> newSinguity);
        void selectMobileUnitsInArea(const Rectangle& area);
        void setSelectedMobileUnitsDestination(const Vector2D& destination);
        std::unordered_set<std::shared_ptr<Singuity>, SharedPointerHash<Singuity>> singuities() const;

    private:
        std::unordered_set<std::shared_ptr<Singuity>, SharedPointerHash<Singuity>> _singuities;
        std::unordered_set<std::shared_ptr<MobileUnit>, SharedPointerHash<MobileUnit>> _mobileUnits;
        std::unordered_set<std::shared_ptr<MobileUnit>, SharedPointerHash<MobileUnit>> _selectedMobileUnits;
    };
}