#pragma once

#include "spawners/Spawner.h"
#include "players/Player.h"

namespace uw
{
    class CompleteGameState
    {
    public:
        CompleteGameState(std::vector<std::shared_ptr<Spawner>>&& spawners, std::vector<std::shared_ptr<Player>>&& players) :
            _spawners(std::forward<std::vector<std::shared_ptr<Spawner>>>(spawners)),
            _players(std::forward<std::vector<std::shared_ptr<Player>>>(players))
        {}

        CompleteGameState(const CompleteGameState& copy)
        {
            for (const auto spawner : copy._spawners)
            {
                _spawners.emplace_back(std::make_shared<Spawner>(*spawner));
            }

            for (const auto player : copy._players)
            {
                _players.emplace_back(std::make_shared<Player>(*player));
            }
        }

        static CompleteGameState empty()
        {
            return CompleteGameState(std::vector<std::shared_ptr<Spawner>>(), std::vector<std::shared_ptr<Player>>());
        }

        std::vector<std::shared_ptr<Player>>& players()
        {
            return _players;
        }

        const std::vector<std::shared_ptr<Player>>& players() const
        {
            return _players;
        }

        std::vector<std::shared_ptr<Spawner>>& spawners()
        {
            return _spawners;
        }

        const std::vector<std::shared_ptr<Spawner>>& spawners() const
        {
            return _spawners;
        }

        void addPlayer(std::shared_ptr<Player> player)
        {
            _players.push_back(player);
        }

        void addSpawners(const std::vector<std::shared_ptr<Spawner>>& spawners)
        {
            _spawners.insert(_spawners.end(), spawners.begin(), spawners.end());
        }

    private:
        std::vector<std::shared_ptr<Spawner>> _spawners;
        std::vector<std::shared_ptr<Player>> _players;
    };
}