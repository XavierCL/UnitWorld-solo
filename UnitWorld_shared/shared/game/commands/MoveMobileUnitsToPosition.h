#pragma once

#include "shared/game/play/CompleteGameState.h"

namespace uw
{
    class MoveMobileUnitsToPosition
    {
    public:

        MoveMobileUnitsToPosition(const xg::Guid& playerId, const std::vector<xg::Guid>& singuityIds, const Vector2D& destination) :
            _playerId(playerId),
            _singuityIds(singuityIds.begin(), singuityIds.end()),
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
        const std::unordered_set<xg::Guid> _singuityIds;
        const Vector2D _destination;
    };
}