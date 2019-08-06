#pragma once

// Set by the window manager on user events
// Get by the game drawer to draw user things on the screen (selected units, selection rectangle, etc)
// Calls the ServerCommander when the user right clicks for example, which in turn calls the server

#include "shared/game/physics/Vector2D.h"
#include "shared/game/geometry/Rectangle.h"

#include "commons/Option.hpp"
#include "commons/Guid.hpp"

#include <vector>

namespace uw
{
    class UserControlState
    {
    public:
        void setUserLeftMouseDownPosition(const Vector2D& position)
        {

        }

        void setUserLeftMouseUpPosition(const Vector2D position)
        {

        }

        void setUserMousePosition(const Vector2D& position)
        {

        }

        void setUserRightMouseDownPosition(const Vector2D& position)
        {

        }

        Option<Rectangle> getSelectionRectangle() const
        {

        }

        std::vector<xg::Guid> getSelectedUnits() const
        {

        }
    };
}