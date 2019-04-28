#pragma once

#include <SFML/Graphics.hpp>

namespace uw
{
    class GraphicalUnit
    {
    public:

        std::shared_ptr<sf::Drawable> drawable() const;

    protected:
        std::shared_ptr<sf::Drawable> _drawable;
    };
}