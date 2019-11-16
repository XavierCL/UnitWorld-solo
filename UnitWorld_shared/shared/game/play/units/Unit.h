#pragma once

#include "shared/game/geometry/Vector2D.h"

#include "commons/Guid.hpp"

namespace uw
{
    class Unit
    {
    public:
        xg::Guid id() const;

        Vector2D position() const;
        void position(const Vector2D& newPosition);

    protected:
        Unit(const xg::Guid& id, const Vector2D& initialPosition);
        Unit(const Vector2D& initialPosition);

    private:
        const xg::Guid _id;
        Vector2D _position;
    };
}