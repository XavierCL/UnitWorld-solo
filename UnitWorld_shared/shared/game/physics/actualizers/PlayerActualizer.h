#pragma once

#include "SinguityActualizer.h"

#include "shared/game/play/players/Player.h"

namespace uw
{
    class PlayerActualizer
    {
    public:

        PlayerActualizer(std::shared_ptr<Player> player) :
            _player(player),
            _singuityActualizers(initializeSinguityActualizers(player))
        {
            _player->addSinguityAddedCallback(singuityAddedCallbackId(), [this](std::shared_ptr<Singuity> singuity) {
                _singuityActualizers.emplace_back(SinguityActualizer(singuity, _player->id()));
            });
        }

        ~PlayerActualizer()
        {
            _player->removeSinguityAddedCallback(singuityAddedCallbackId());
        }

        void updateCollisions(std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<CollisionDetector>>> collisionDetectorsByPlayerId, std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<UnitWithHealthPoint>>> shootablesById)
        {
            for (auto& singuityActualizer : _singuityActualizers)
            {
                singuityActualizer.updateCollisions(_player->id(), collisionDetectorsByPlayerId, shootablesById);
            }
        }

        void shootEnemies(std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<UnitWithHealthPoint>>> shootablesById, const unsigned long long& frameTimestamp)
        {
            for (auto& singuityActualizer : _singuityActualizers)
            {
                singuityActualizer.shootEnemy(shootablesById, frameTimestamp);
            }
        }

        void removeSinguitiesAndUpdateTheirPhysics(const std::unordered_map<xg::Guid, std::shared_ptr<Spawner>>& spawnersById, std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<UnitWithHealthPoint>>> shootablesById)
        {
            auto aliveSinguities(std::make_shared<std::vector<std::shared_ptr<Singuity>>>());
            for (auto& singuityActualizer : _singuityActualizers)
            {
                if (!singuityActualizer.singuity()->isDead())
                {
                    singuityActualizer.actualize(spawnersById, shootablesById);

                    // Singuities can kill themselves
                    if (!singuityActualizer.singuity()->isDead())
                    {
                        aliveSinguities->emplace_back(singuityActualizer.singuity());
                    }
                }
            }

            _player->setSinguities(aliveSinguities);
        }

    private:

        static std::vector<SinguityActualizer> initializeSinguityActualizers(std::shared_ptr<Player> player)
        {
            std::vector<SinguityActualizer> singuityActualizers;
            singuityActualizers.reserve(player->singuities()->size());
            for (const auto& singuity : *player->singuities())
            {
                singuityActualizers.emplace_back(singuity, player->id());
            }
            return singuityActualizers;
        }

        static xg::Guid singuityAddedCallbackId()
        {
            return xg::Guid("1d453598-7b38-411e-b99d-93c79ad8736c");
        }

        const std::shared_ptr<Player> _player;
        std::vector<SinguityActualizer> _singuityActualizers;
    };
}