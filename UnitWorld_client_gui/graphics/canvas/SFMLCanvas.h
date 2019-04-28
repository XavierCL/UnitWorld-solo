#pragma once

#include "DisplayDrawerCanvas.h"

#include <SFML/Graphics.hpp>

namespace uw
{
    class SFMLCanvas : public DisplayDrawerCanvas
    {
    public:
        SFMLCanvas(sf::RenderWindow& canvas) :
            _windowCanvas(canvas)
        {
            _windowCanvas.setActive();
        }

        void draw(const sf::Drawable& drawable, const sf::RenderStates& states = sf::RenderStates::Default)
        {
            _windowCanvas.draw(drawable, states);
        }

        void draw(const sf::Vertex* vertices, std::size_t vertexCount,
            sf::PrimitiveType type, const sf::RenderStates& states = sf::RenderStates::Default)
        {
            _windowCanvas.draw(vertices, vertexCount, type, states);
        }

        void draw(const std::string& text)
        {
            sf::Text graphic_text;
            graphic_text.setString(text);
            graphic_text.setFillColor(sf::Color::Green);
            _windowCanvas.draw(graphic_text);
        }

        void display()
        {
            _windowCanvas.display();
            _windowCanvas.clear();
        }

        void close()
        {
            _windowCanvas.close();
        }

    private:

        sf::RenderWindow& _windowCanvas;
    };
}