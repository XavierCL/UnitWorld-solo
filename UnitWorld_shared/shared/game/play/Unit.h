#pragma once

#include "../physics/Vector2D.h"

#include "commons/Guid.hpp"

namespace uw
{
    class Unit
    {
    public:
        xg::Guid id() const;

        virtual void actualize() = 0;

        Vector2D position() const;
        void position(const Vector2D& newPosition);

        struct SharedUnitHash
        {
            std::size_t operator()(std::shared_ptr<Unit> const &unit) const
            {
                return _guidHash(unit->id());
            }

        private:
            std::hash<xg::Guid> _guidHash;
        };

        struct SharedUnitEqual
        {
            bool operator()(std::shared_ptr<Unit> const &first, std::shared_ptr<Unit> const &second) const
            {
                return first->id() == second->id();
            }
        };

    protected:
        Unit(const xg::Guid& id, const Vector2D& initialPosition);
        Unit(const Vector2D& initialPosition);

    private:
        const xg::Guid _id;
        Vector2D _position;
    };
}