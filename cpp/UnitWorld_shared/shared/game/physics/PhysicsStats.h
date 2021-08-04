#pragma once

#include "commons/Logger.hpp"

#include <string>

namespace uw
{
    class PhysicsStats
    {
    public:
        void feedFrameDuration(const unsigned int& millisecond)
        {
            _summedMillis += millisecond;
            ++_currentFrameCount;
            if (_currentFrameCount % frameBeforeLog() == 0)
            {
                Logger::trace("FrameCount: " + std::to_string(_currentFrameCount) + ". Average Frame Time of past " + std::to_string(frameBeforeLog()) + " ms: " + std::to_string(_summedMillis / frameBeforeLog()) + "ms.");
                _summedMillis = 0;
            }
        }

    private:

        static unsigned int frameBeforeLog()
        {
            return 30;
        }

        double _summedMillis;
        unsigned long _currentFrameCount;
    };
}