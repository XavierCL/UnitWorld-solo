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

        void spawnAll(std::unordered_map<xg::Guid, std::shared_ptr<Player>>& playerById, const long long& frameCount)
        {
            for (auto& spawnerActualizer : _spawnerActualizers)
            {
                spawnerActualizer.spawn(playerById, frameCount);
            }
        }

        void updateCollisions(std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<CollisionDetector>>> collisionDetectorsByPlayerId, std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<UnitWithHealthPoint>>> shootablesById, std::shared_ptr<CollisionDetector> spawnerCollisionDetector, const std::unordered_map<xg::Guid, std::shared_ptr<Spawner>>& spawnersById)
        {
            for (auto& playerActualizer : _playerActualizers)
            {
                playerActualizer.updateCollisions(collisionDetectorsByPlayerId, shootablesById, spawnerCollisionDetector, spawnersById, _completeGameState->frameCount());
            }
        }

        void shootEnemies(std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<UnitWithHealthPoint>>> shootablesById)
        {
            for (auto& playerActualizer : _playerActualizers)
            {
                playerActualizer.shootEnemies(shootablesById, _completeGameState->frameCount());
            }
        }

        void updatePhysics(const std::unordered_map<xg::Guid, std::shared_ptr<Spawner>>& spawnersById, std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<UnitWithHealthPoint>>> shootablesById)
        {
            for (auto& playerActualizer : _playerActualizers)
            {
                playerActualizer.removeSinguitiesAndUpdateTheirPhysics(spawnersById, shootablesById);
            }
        }

        void updateSpawnerAllegences()
        {
            for (auto& spawnerActualizer : _spawnerActualizers)
            {
                spawnerActualizer.updateAllegence(_completeGameState->frameCount());
            }
        }

    private:

        static std::vector<SpawnerActualizer> initializeSpawnerActualizers(std::shared_ptr<CompleteGameState> completeGameState)
        {
            std::vector<SpawnerActualizer> spawnerActualizers;
            spawnerActualizers.reserve(completeGameState->spawners().size());
            for (const auto& spawner : completeGameState->spawners())
            {
                spawnerActualizers.emplace_back(spawner);
            }
            return spawnerActualizers;
        }

        static std::vector<PlayerActualizer> initializePlayerActualizers(std::shared_ptr<CompleteGameState> completeGameState)
        {
            std::vector<PlayerActualizer> playerActualizers;
            playerActualizers.reserve(completeGameState->players().size());
            for (const auto& player : completeGameState->players())
            {
                playerActualizers.emplace_back(player);
            }
            return playerActualizers;
        }

        const std::shared_ptr<CompleteGameState> _completeGameState;
        std::vector<SpawnerActualizer> _spawnerActualizers;
        std::vector<PlayerActualizer> _playerActualizers;
    };
}