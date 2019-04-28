#pragma once

#include <SFML/Graphics.hpp>

namespace uw
{
    class GraphicalUnit
    {
    public:
        GraphicalUnit()
        {}

        std::shared_ptr<sf::Drawable> drawable() const
        {
            return _drawable;
        }

    protected:
        std::shared_ptr<sf::Drawable> _drawable;
    };
}