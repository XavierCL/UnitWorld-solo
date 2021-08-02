#pragma once

#include "commons/Logger.hpp"

#include <string>

namespace uw
{
    class WindowStats
    {
    public:
        void feedFrameDuration(const unsigned int& millisecond)
        {
            _summedMillis += millisecond;
            ++_currentFrameCount;
            if (_currentFrameCount % frameBeforeLog() == 0)
            {
                Logger::trace("Window frame: " + std::to_string(_currentFrameCount) + ". Average time of past " + std::to_string(frameBeforeLog()) + " frames ms: " + std::to_string(_summedMillis / frameBeforeLog()));
                _summedMillis = 0;
            }
        }

    private:

        static unsigned int frameBeforeLog()
        {
            return 26;
        }

        unsigned int _summedMillis;
        unsigned long _currentFrameCount;
    };
}