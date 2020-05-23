#pragma once

#include "shared/game/play/spawners/Spawner.h"
#include "shared/game/play/players/Player.h"

#include "commons/CollectionPipe.h"

namespace uw
{
    class SpawnerActualizer
    {
    public:
        SpawnerActualizer(std::shared_ptr<Spawner> spawner) :
            _spawner(spawner)
        {}

        void spawn(std::unordered_map<xg::Guid, std::shared_ptr<Player>>& playerById, const long long& frameCount)
        {
            _spawner->spawnIfCan([&playerById](const xg::Guid& playerId, std::shared_ptr<Singuity> spawnedSinguity) {
                (&playerById | find<std::shared_ptr<Player>>(playerId)).foreach([spawnedSinguity](std::shared_ptr<Player> player) {
                    player->addSinguity(spawnedSinguity);
                });
            }, frameCount);
        }

        void updateAllegence(const long long& frameCount)
        {
            _spawner->updateAllegence(frameCount);
        }

    private:
        const std::shared_ptr<Spawner> _spawner;
    };
}