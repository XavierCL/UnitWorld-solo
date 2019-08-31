#pragma once

#include "Spawner.h"
#include "Player.h"

namespace uw
{
    class CompleteGameState
    {
    public:
        CompleteGameState(std::vector<std::shared_ptr<Spawner>>&& spawners, std::vector<std::shared_ptr<Player>>&& players) :
            _spawners(std::forward<std::vector<std::shared_ptr<Spawner>>>(spawners)),
            _players(std::forward<std::vector<std::shared_ptr<Player>>>(players))
        {}

        static CompleteGameState empty()
        {
            return CompleteGameState(std::vector<std::shared_ptr<Spawner>>(), std::vector<std::shared_ptr<Player>>());
        }

        std::vector<std::shared_ptr<Player>> players() const
        {
            return _players;
        }

        std::vector<std::shared_ptr<Spawner>> spawners() const
        {
            return _spawners;
        }

        void addPlayer(std::shared_ptr<Player> player)
        {
            _players.push_back(player);
        }

    private:
        std::vector<std::shared_ptr<Spawner>> _spawners;
        std::vector<std::shared_ptr<Player>> _players;
    };
}