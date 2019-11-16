#pragma once

#include "shared/game/play/units/Singuity.h"
#include "shared/game/play/units/SpawnerDestination.h"

#include "shared/game/geometry/Vector2D.h"

#include "commons/Guid.hpp"

#include <immer/map.hpp>
#include <immer/set.hpp>

namespace uw
{
    class Player
    {
    public:
        Player(const xg::Guid& id, const std::vector<std::shared_ptr<Singuity>>& singuities);
        Player(const Player& other);

        xg::Guid id() const;
        void setSinguitiesDestination(const immer::set<xg::Guid>& singuitiesId, const Vector2D& destination);
        void setSinguitiesSpawnerDestination(const immer::set<xg::Guid>& singuitiesId, const SpawnerDestination& spawnerDestination);
        void addSinguity(std::shared_ptr<Singuity> newSinguity);
        void setSinguities(std::shared_ptr<std::vector<std::shared_ptr<Singuity>>> singuities);
        std::shared_ptr<std::vector<std::shared_ptr<Singuity>>> singuities() const;

        void addSinguityAddedCallback(const xg::Guid& callbackId, const std::function<void(std::shared_ptr<Singuity>)>& callback);
        void removeSinguityAddedCallback(const xg::Guid& callbackId);

    private:
        const xg::Guid _id;

        std::shared_ptr<std::vector<std::shared_ptr<Singuity>>> _singuities;

        immer::map<xg::Guid, std::function<void(std::shared_ptr<Singuity>)>> _singuityAddedCallbacks;
    };
}