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
            _singuityActualizers(initializeSinguityActualizers(player)),
            _singuityAddedCallbackId(xg::newGuid())
        {
            _player->addSinguityAddedCallback(_singuityAddedCallbackId, [this](std::shared_ptr<Singuity> singuity) {
                _singuityActualizers->emplace_back(std::make_shared<SinguityActualizer>(singuity, _player->id()));
            });
        }

        ~PlayerActualizer()
        {
            _player->removeSinguityAddedCallback(_singuityAddedCallbackId);
        }

        void updateCollisions(std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<CollisionDetector>>> collisionDetectorsByPlayerId, std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<UnitWithHealthPoint>>> shootablesById)
        {
            for (auto& singuityActualizer : *_singuityActualizers)
            {
                singuityActualizer->updateCollisions(_player->id(), collisionDetectorsByPlayerId, shootablesById);
            }
        }

        void shootEnemies(std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<UnitWithHealthPoint>>> shootablesById, const unsigned long long& frameCount)
        {
            for (auto& singuityActualizer : *_singuityActualizers)
            {
                singuityActualizer->shootEnemy(shootablesById, frameCount);
            }
        }

        void removeSinguitiesAndUpdateTheirPhysics(const std::unordered_map<xg::Guid, std::shared_ptr<Spawner>>& spawnersById, std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<UnitWithHealthPoint>>> shootablesById)
        {
            auto aliveSinguities(std::make_shared<std::vector<std::shared_ptr<Singuity>>>());
            auto aliveSinguityActualizers(std::make_shared<std::vector<std::shared_ptr<SinguityActualizer>>>());

            aliveSinguities->reserve(_player->singuities()->size());
            aliveSinguityActualizers->reserve(_singuityActualizers->size());

            for (auto& singuityActualizer : *_singuityActualizers)
            {
                if (!singuityActualizer->singuity()->isDead())
                {
                    singuityActualizer->actualize(spawnersById, shootablesById);

                    // Singuities can kill themselves
                    if (!singuityActualizer->singuity()->isDead())
                    {
                        aliveSinguities->emplace_back(singuityActualizer->singuity());
                        aliveSinguityActualizers->emplace_back(singuityActualizer);
                    }
                }
            }

            _player->setSinguities(aliveSinguities);
            _singuityActualizers = aliveSinguityActualizers;
        }

    private:

        static std::shared_ptr<std::vector<std::shared_ptr<SinguityActualizer>>> initializeSinguityActualizers(std::shared_ptr<Player> player)
        {
            auto singuityActualizers = std::make_shared<std::vector<std::shared_ptr<SinguityActualizer>>>();
            singuityActualizers->reserve(player->singuities()->size());
            for (const auto& singuity : *player->singuities())
            {
                singuityActualizers->emplace_back(std::make_shared<SinguityActualizer>(singuity, player->id()));
            }
            return singuityActualizers;
        }

        const std::shared_ptr<Player> _player;
        const xg::Guid _singuityAddedCallbackId;
        std::shared_ptr<std::vector<std::shared_ptr<SinguityActualizer>>> _singuityActualizers;
    };
}