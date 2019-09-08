#pragma once

#include "GameCommand.h"

#include "shared/game/play/CompleteGameState.h"

#include "shared/game/play/units/SpawnerDestination.h"

#include "commons/Guid.hpp"

#include <immer/set.hpp>

namespace uw
{
    class MoveMobileUnitsToSpawner: public GameCommand
    {
    public:
        MoveMobileUnitsToSpawner(const xg::Guid& playerId, const immer::set<xg::Guid>& mobileUnitIds, const xg::Guid& spawnerId):
            _playerId(playerId),
            _singuityIds(mobileUnitIds),
            _spawnerId(spawnerId)
        {}

        virtual void execute(std::shared_ptr<CompleteGameState> completeGameState) const
        {
            for (std::shared_ptr<Spawner> spawner : completeGameState->spawners())
            {
                if (spawner->id() == _spawnerId)
                {
                    if (spawner->canBeReguvanatedBy(_playerId))
                    {
                        for (auto player : completeGameState->players())
                        {
                            if (player->id() == _playerId)
                            {
                                player->setSinguitiesSpawnerDestination(_singuityIds, SpawnerDestination(spawner->id(), spawner->allegence()));
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }

    private:
        const xg::Guid _playerId;
        const immer::set<xg::Guid> _singuityIds;
        const xg::Guid _spawnerId;
    };
}