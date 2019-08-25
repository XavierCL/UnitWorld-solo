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

        CollidablePoint(const CollidablePoint& copy) :
            _id(copy._id),
            _position(copy._position)
        {}

        xg::Guid id() const
        {
            return _id;
        }

        Vector2D position() const
        {
            return _position;
        }

    private:
        const xg::Guid _id;
        const Vector2D _position;
    };
}