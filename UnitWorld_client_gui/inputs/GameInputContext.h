#pragma once

#include "game/physics/Vector2.h"

#include "utils/Option.hpp"

namespace uw
{
    class GameInputContext
    {
    public:
        GameInputContext() :
            _clickedAtFrameOffset(1),
            _clickedAt(),
            _clickedFromFrameOffset(1)
        {}
        void resetFrame()
        {
            ++_clickedAtFrameOffset;
        }
        const unsigned int clickedFrameOffset() const
        {
            return _clickedAtFrameOffset;
        }
        void clickedAt(const Vector2 &at)
        {
            _clickedAtFrameOffset = 0;
            _clickedAt = Option<Vector2>(at);
        }
        const Option<Vector2> clickedAt() const
        {
            return _clickedAt;
        }
        const unsigned int clickedFromFrameOffset() const
        {
            return _clickedFromFrameOffset;
        }
        void clicked
    private:
        unsigned int _clickedAtFrameOffset;
        Option<Vector2> _clickedAt;
        unsigned int _clickedFromFrameOffset;
    };
}