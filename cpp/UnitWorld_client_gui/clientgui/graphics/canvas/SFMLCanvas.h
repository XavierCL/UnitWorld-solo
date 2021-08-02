#pragma once

#include "DisplayCanvas.h"
#include "SFMLDrawingCanvas.h"

#include <SFML/Graphics.hpp>

namespace uw
{
    class SFMLCanvas : public DisplayCanvas, public SFMLDrawingCanvas
    {
    public:
        SFMLCanvas(std::shared_ptr<sf::RenderWindow> canvas);

        void draw(const sf::Drawable& drawable, const sf::RenderStates& states = sf::RenderStates::Default);

        void draw(const sf::Vertex* vertices, std::size_t vertexCount, sf::PrimitiveType type, const sf::RenderStates& states = sf::RenderStates::Default);

        void draw(const std::string& text);

        void display();

        void close();

    private:

        std::shared_ptr<sf::RenderWindow> _windowCanvas;
    };
}