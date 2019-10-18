#pragma once

#include "SpawnerDestination.h"

#include "shared/game/geometry/Vector2D.h"

#include "commons/CollectionPipe.h"
#include "commons/Guid.hpp"

#include <variant>

namespace uw
{
    class MobileUnitDestination
    {
    public:

        MobileUnitDestination(const Vector2D& destination):
            _destination(destination)
        {}

        MobileUnitDestination(const SpawnerDestination& destination):
            _destination(destination)
        {}

        MobileUnitDestination(const xg::Guid& spawnerId):
            _destination(spawnerId)
        {}

        bool isSpawnerDestination() const
        {
            return std::visit(overloaded{
                [](const Vector2D& positionDestination) { return false; },
                [](const SpawnerDestination& spawnerDestination) { return true; },
                [](const xg::Guid& inconditionalSpawnerDestination) { return true; }
            }, _destination);
        }

        template <typename Return, typename VectorMapping, typename SpawnerDestinationMapping, typename UnconditionalSpawnerDestinationMapping>
        Return map(const VectorMapping& vectorMapping, const SpawnerDestinationMapping& spawnerDestinationMapping, const UnconditionalSpawnerDestinationMapping& unconditionalSpawnerDestinationMapping) const
        {
            return std::visit(overloaded{
                vectorMapping,
                spawnerDestinationMapping,
                unconditionalSpawnerDestinationMapping
            }, _destination);
        }

    private:
        const std::variant<Vector2D, SpawnerDestination, xg::Guid> _destination;
    };
}