#pragma once

#include "GameCommand.h"

#include "shared/game/play/CompleteGameState.h"

#include <immer/set.hpp>

namespace uw
{
    class SetSpawnersRally: public GameCommand
    {
    public:

        SetSpawnersRally(const xg::Guid& playerId, const immer::set<xg::Guid>& spawnersId, const MobileUnitDestination& rally) :
            _playerId(playerId),
            _spawnersId(spawnersId),
            _rally(rally)
        {}

        void execute(std::shared_ptr<CompleteGameState> completeGameState) const
        {
            for (auto spawner : completeGameState->spawners())
            {
                if (spawner->isAllegedToPlayer(_playerId) && _spawnersId.count(spawner->id()) > 0)
                {
                    spawner->setRally(_rally);
                }
            }
        }

    private:
        const xg::Guid _playerId;
        const immer::set<xg::Guid> _spawnersId;
        const MobileUnitDestination _rally;
    };
}