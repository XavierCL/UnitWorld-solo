#pragma once

#include "SpawnerActualizer.h"
#include "PlayerActualizer.h"

#include "shared/game/play/CompleteGameState.h"

namespace uw
{
    class CompleteGameStateActualizer
    {
    public:
        CompleteGameStateActualizer(std::shared_ptr<CompleteGameState> completeGameState):
            _completeGameState(completeGameState),
            _spawnerActualizers(initializeSpawnerActualizers(completeGameState)),
            _playerActualizers(initializePlayerActualizers(completeGameState))
        {}

        void spawnAll(std::unordered_map<xg::Guid, std::shared_ptr<Player>>& playerById, const unsigned long long& frameTimestamp)
        {
            for (auto& spawnerActualizer : _spawnerActualizers)
            {
                spawnerActualizer.spawn(playerById, frameTimestamp);
            }
        }

        void updateShootingAndPhysicsPredictions(std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<CollisionDetector>>> collisionDetectorsByPlayerId, std::shared_ptr<CollisionDetector> neutralCollisionDetector, std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<UnitWithHealthPoint>>> shootablesById, const unsigned long long& frameTimestamp)
        {
            for (auto& playerActualizer : _playerActualizers)
            {
                playerActualizer.updateShootingAndRepulsionForces(collisionDetectorsByPlayerId, neutralCollisionDetector, shootablesById, frameTimestamp);
            }
        }

        void updatePhysics()
        {
            for (auto& playerActualizer : _playerActualizers)
            {
                playerActualizer.removeSinguitiesAndUpdateTheirPhysics();
            }
        }

        void updateSpawnerAllegences()
        {
            for (auto& spawnerActualizer : _spawnerActualizers)
            {
                spawnerActualizer.updateAllegence();
            }
        }

    private:

        static std::vector<SpawnerActualizer> initializeSpawnerActualizers(std::shared_ptr<CompleteGameState> completeGameState)
        {
            auto spawnerActualizers =
                &completeGameState->spawners()
                | map<SpawnerActualizer>([](std::shared_ptr<Spawner> spawner) {
                    return SpawnerActualizer(spawner);
                }) | toVector<SpawnerActualizer>();

            return *spawnerActualizers;
        }

        static std::vector<PlayerActualizer> initializePlayerActualizers(std::shared_ptr<CompleteGameState> completeGameState)
        {
            auto playerActualizers =
                &completeGameState->players()
                | map<PlayerActualizer>([](std::shared_ptr<Player> player) {
                return PlayerActualizer(player);
            }) | toVector<PlayerActualizer>();

            return *playerActualizers;
        }

        const std::shared_ptr<CompleteGameState> _completeGameState;
        std::vector<SpawnerActualizer> _spawnerActualizers;
        std::vector<PlayerActualizer> _playerActualizers;
    };
}