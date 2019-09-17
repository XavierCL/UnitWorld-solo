#include "SFMLCanvas.h"

using namespace uw;

SFMLCanvas::SFMLCanvas(std::shared_ptr<sf::RenderWindow> canvas) :
    _windowCanvas(canvas)
{}

void SFMLCanvas::draw(const sf::Drawable& drawable, const sf::RenderStates& states)
{
    _windowCanvas->draw(drawable, states);
}

void SFMLCanvas::draw(const sf::Vertex* vertices, std::size_t vertexCount, sf::PrimitiveType type, const sf::RenderStates& states)
{
    _windowCanvas->draw(vertices, vertexCount, type, states);
}

void SFMLCanvas::draw(const std::string& text)
{
    sf::Text graphic_text;
    graphic_text.setString(text);
    graphic_text.setFillColor(sf::Color::Green);
    _windowCanvas->draw(graphic_text);
}

void SFMLCanvas::display()
{
    _windowCanvas->display();
    _windowCanvas->clear();
}

void SFMLCanvas::close()
{
    _windowCanvas->close();
}