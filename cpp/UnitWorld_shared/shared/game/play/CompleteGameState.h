#pragma once

#include "spawners/Spawner.h"
#include "players/Player.h"

namespace uw
{
    class CompleteGameState
    {
    public:
        CompleteGameState(std::vector<std::shared_ptr<Spawner>>&& spawners, std::vector<std::shared_ptr<Player>>&& players, const long long frameCount) :
            _spawners(std::forward<std::vector<std::shared_ptr<Spawner>>>(spawners)),
            _players(std::forward<std::vector<std::shared_ptr<Player>>>(players)),
            _frameCount(frameCount),
            _version(xg::newGuid())
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

            _frameCount = copy._frameCount;
            _version = copy._version;
        }

        static CompleteGameState empty()
        {
            return CompleteGameState(std::vector<std::shared_ptr<Spawner>>(), std::vector<std::shared_ptr<Player>>(), 0);
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
            _version = xg::newGuid();
        }

        void addSpawners(const std::vector<std::shared_ptr<Spawner>>& spawners)
        {
            _spawners.insert(_spawners.end(), spawners.begin(), spawners.end());
            _version = xg::newGuid();
        }

        long long frameCount() const
        {
            return _frameCount;
        }

        void incrementFrameCount()
        {
            ++_frameCount;
            _version = xg::newGuid();
        }

        xg::Guid version() const
        {
            return _version;
        }

    private:
        std::vector<std::shared_ptr<Spawner>> _spawners;
        std::vector<std::shared_ptr<Player>> _players;
        long long _frameCount;
        xg::Guid _version;
    };
}