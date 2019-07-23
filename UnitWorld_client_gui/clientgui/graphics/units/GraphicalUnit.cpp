#include "GraphicalUnit.h"

using namespace uw;

std::shared_ptr<sf::Drawable> GraphicalUnit::drawable() const
{
    return _drawable;
}