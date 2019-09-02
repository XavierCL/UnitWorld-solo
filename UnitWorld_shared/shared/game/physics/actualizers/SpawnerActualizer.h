#pragma once

#include "shared/game/play/Spawner.h"
#include "shared/game/play/Player.h"

#include "commons/CollectionPipe.h"

namespace uw
{
    class SpawnerActualizer
    {
    public:
        SpawnerActualizer(std::shared_ptr<Spawner> spawner) :
            _spawner(spawner)
        {}

        void spawn(std::unordered_map<xg::Guid, std::shared_ptr<Player>>& playerById, const unsigned long long& frameTimestamp)
        {
            _spawner->spawnIfCan([&playerById](const xg::Guid& playerId, std::shared_ptr<Singuity> spawnedSinguity) {
                (playerById | find<std::shared_ptr<Player>>(playerId)).foreach([spawnedSinguity](std::shared_ptr<Player> player) {
                    player->addSinguity(spawnedSinguity);
                });
            }, frameTimestamp);
        }

        void updateAllegence()
        {
            _spawner->updateAllegence();
        }

    private:
        const std::shared_ptr<Spawner> _spawner;
    };
}