#pragma once
#pragma once

#include "clientAis/ais/Artificial.h"
#include <shared\game\physics\collisions\KdtreeCollisionDetectorFactory.h>

namespace uw
{
    class Microer2 : public Artificial
    {
    public:
        Microer2() :
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
            for (auto player : players)
            {
                if (player->id() != myself->id())
                {
                    enemies.emplace_back(player);
                }
            }               // creating the enemy players' vector
            Vector2D nextUnitsPosition = Vector2D();

            std::unordered_set<xg::Guid> mySinguities;
            for (auto singuity : *myself->singuities())
            {
                mySinguities.emplace(singuity->id());
            }               // putting all my singuities in the selected singuities vector

            std::vector<std::shared_ptr<Singuity>> enemySinguities;
            for (auto player : enemies)
            {
                for (auto singuity : *(player->singuities()))
                {
                    enemySinguities.emplace_back(singuity);
                }
            }               // putting all the enemy's singuities in the enemySinguities vector

            Vector2D averagePositionOfSinguities = Vector2D();
            for (auto singuity : *myself->singuities())
            {
                averagePositionOfSinguities += singuity->position();
            }
            averagePositionOfSinguities = averagePositionOfSinguities / myself->singuities()->size();
            // average position of all my singuities

            Vector2D closestEnemySinguityPosition = Vector2D(INT32_MAX, INT32_MAX);
            for (auto enemySinguity : enemySinguities)
            {
                if ((averagePositionOfSinguities - closestEnemySinguityPosition).moduleSq() > (averagePositionOfSinguities - enemySinguity->position()).moduleSq())
                {
                    closestEnemySinguityPosition = enemySinguity->position();
                }
            }
            // finding the closest enemy singuity

            // microing (ish)
            if (closestEnemySinguityPosition.distanceSq(averagePositionOfSinguities) > 5000)
            {
                nextUnitsPosition = closestEnemySinguityPosition;
            }
            else
            {
                nextUnitsPosition = averagePositionOfSinguities + ((closestEnemySinguityPosition - averagePositionOfSinguities).atModule(10));
            }

            serverCommander->moveUnitsToPosition(mySinguities, nextUnitsPosition);
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

