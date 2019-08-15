#pragma once

#include "Vector2D.h"

#include "commons/Guid.hpp"

namespace uw
{
    class CollidablePoint
    {
    public:
        CollidablePoint(const xg::Guid& id, const Vector2D& position) :
            _id(id),
            _position(position)
        {}

    private:
        const xg::Guid _id;
        const Vector2D _position;
    };
}