#pragma once

#include "clientAis/ais/Artificial.h"
#include <shared\game\physics\collisions\KdtreeCollisionDetectorFactory.h>

namespace uw
{
    class Microer : public Artificial
    {
    public:
        Microer():
            _collisionDetectorFactory(std::make_shared<KdtreeCollisionDetectorFactory>())
        {}

        virtual void frameHappened(std::shared_ptr<GameManager> gameManager, std::shared_ptr<ServerCommander> serverCommander)
        {
            auto myself = gameManager->currentPlayer().getOrElse(std::shared_ptr<Player>());
            if (!myself)
            {
                return;
            }               // if I don't have a currentPlayer, then do nothing and return

            auto spawners = gameManager->completeGameState()->spawners();   // all spawners
            auto players = gameManager->completeGameState()->players();     // all players
            std::vector<std::shared_ptr<Player>> enemies;                   // enemy players
            for (auto player: players)
            {
                if (player->id() != myself->id())
                {
                    enemies.emplace_back(player);
                }
            }               // creating the enemy players vector

            std::unordered_set<xg::Guid> selectedSinguities;
            for (auto singuity : *myself->singuities())
            {
                selectedSinguities.emplace(singuity->id());
            }               // putting all my singuities in the selected singuities vector

            std::vector<std::shared_ptr<Spawner>> wantedSpawners;
            for (auto spawner: spawners)
            {
                if (!spawner->isClaimedBy(myself->id()))
                {
                    wantedSpawners.emplace_back(spawner);
                }
            }               // putting all spawners that I want in the vector

            auto collisionDetector = getCollisionDetectorForSpawners(wantedSpawners);   // collision detector of wanted spawners

            Vector2D averagePositionOfSinguities = Vector2D();
            for (auto singuity: *myself->singuities())
            {
                averagePositionOfSinguities += singuity->position();
            }
            averagePositionOfSinguities = averagePositionOfSinguities / myself->singuities()->size();

            // average position of all my singuities

            auto spawnerDestinationID = collisionDetector->getClosest(CollidablePoint(xg::Guid(), averagePositionOfSinguities)).getOrElse(xg::Guid());

            serverCommander->moveUnitsToSpawner(selectedSinguities, spawnerDestinationID);
        }

    private:
        std::shared_ptr<CollisionDetector> getCollisionDetectorForSpawners(const std::vector<std::shared_ptr<Spawner>>& spawners)
        {
            std::vector<CollidablePoint> collidablePoints{};

            for (const auto spawner : spawners)
            {
                collidablePoints.emplace_back(spawner->id(), spawner->position());
            }

            auto collisionDetector = _collisionDetectorFactory->create();
            collisionDetector->updateAllCollidablePoints(collidablePoints);

            return collisionDetector;
        }

        const std::shared_ptr<CollisionDetectorFactory> _collisionDetectorFactory;
    };
}

