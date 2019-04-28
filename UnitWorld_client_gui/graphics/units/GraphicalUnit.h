#pragma once

#include <SFML/Graphics.hpp>

namespace uw
{
    class GraphicalUnit
    {
    public:
        GraphicalUnit() :
            _drawable(nullptr)
        {}

        ~GraphicalUnit()
        {
            delete _drawable;
        }

        sf::Drawable& drawable() const
        {
            return *_drawable;
        }

    protected:
        sf::Drawable* _drawable;
    };
}