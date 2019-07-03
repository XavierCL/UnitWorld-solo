#pragma once

#include "play/Player.h"

#include <immer/vector.hpp>

#include <memory>
#include <ctime>

namespace uw
{
    class GameManager
    {
    public:
        GameManager(const unsigned int& framePerSecond):
            _msPerFrame(1000 / framePerSecond)
        {}

        void startSync()
        {
            _isRunning = true;

            while(_isRunning) {
                const auto startFrameTime = clock();

                auto localPlayers = _players;

                for (auto processingPlayer : localPlayers)
                {
                    processingPlayer->actualize();
                }

                const auto endFrameTime = clock();

                const auto frameTimeInMs = (endFrameTime - startFrameTime) / (CLOCK_PER_SEC / 1000);

                if (frameTimeInMs < _msPerFrame)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(_msPerFrame - frameTimeInMs));
                }
            }
        }

        void stop()
        {
            _isRunning = false;
        }

        void addPlayer(std::shared_ptr<Player> newPlayer)
        {
            _players = _players.push_back(newPlayer);
        }

        immer::vector<std::shared_ptr<const Player>> threadSafePlayers()
        {
            return _playersView;
        }

    private:
        const unsigned int _msPerFrame;
        immer::vector<std::shared_ptr<Player>> _players;
        immer::vector<std::shared_ptr<const Player>> _playersView;
        bool _isRunning;
    };
}