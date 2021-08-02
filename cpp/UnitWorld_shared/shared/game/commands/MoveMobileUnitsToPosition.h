#pragma once

#include "GameCommand.h"

#include "shared/game/play/CompleteGameState.h"

#include <immer/set.hpp>

namespace uw
{
    class MoveMobileUnitsToPosition: public GameCommand
    {
    public:

        MoveMobileUnitsToPosition(const xg::Guid& playerId, const immer::set<xg::Guid>& singuityIds, const Vector2D& destination) :
            _playerId(playerId),
            _singuityIds(singuityIds),
            _destination(destination)
        {}

        void execute(std::shared_ptr<CompleteGameState> completeGameState) const
        {
            for (auto player : completeGameState->players())
            {
                if (player->id() == _playerId)
                {
                    player->setSinguitiesDestination(_singuityIds, _destination);
                    break;
                }
            }
        }

    private:
        const xg::Guid _playerId;
        const immer::set<xg::Guid> _singuityIds;
        const Vector2D _destination;
    };
}