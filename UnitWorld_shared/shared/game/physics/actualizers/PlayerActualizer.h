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
                _singuityActualizers.emplace_back(SinguityActualizer(singuity));
            });
        }

        PlayerActualizer(const PlayerActualizer& playerActualizer) = delete;

        ~PlayerActualizer()
        {
            _player->removeSinguityAddedCallback(singuityAddedCallbackId());
        }

        void updateShootingAndRepulsionForces(std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<CollisionDetector>>> collisionDetectorsByPlayerId, std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<UnitWithHealthPoint>>> shootablesById, const unsigned long long& frameTimestamp)
        {
            for (auto& singuityActualizer : _singuityActualizers)
            {
                singuityActualizer.updateShootingAndRepulsionForce(_player->id(), collisionDetectorsByPlayerId, shootablesById, frameTimestamp);
            }
        }

        void removeSinguitiesAndUpdateTheirPhysics(const std::unordered_map<xg::Guid, std::shared_ptr<Spawner>>& spawnersById)
        {
            auto aliveSinguities(std::make_shared<std::vector<std::shared_ptr<Singuity>>>());
            for (auto& singuityActualizer : _singuityActualizers)
            {
                if (!singuityActualizer.singuity()->isDead())
                {
                    singuityActualizer.actualize(_player->id(), spawnersById);

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
            std::shared_ptr<std::vector<SinguityActualizer>> actualizers =
                player->singuities()
                | map<SinguityActualizer>([](std::shared_ptr<Singuity> singuity) {
                    return SinguityActualizer(singuity);
                }) | toVector<SinguityActualizer>();

            return *actualizers;
        }

        static xg::Guid singuityAddedCallbackId()
        {
            return xg::Guid("1d453598-7b38-411e-b99d-93c79ad8736c");
        }

        const std::shared_ptr<Player> _player;
        std::vector<SinguityActualizer> _singuityActualizers;
    };
}