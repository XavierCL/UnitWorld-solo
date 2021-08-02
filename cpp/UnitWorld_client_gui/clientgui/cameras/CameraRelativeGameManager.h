#pragma once

#include "Camera.h"

#include "shared/game/play/units/Singuity.h"
#include "shared/game/play/spawners/Spawner.h"

#include "shared/game/geometry/Circle.h"

#include <memory>

namespace uw
{
    class CameraRelativeGameManager
    {
    public:
        CameraRelativeGameManager(std::shared_ptr<Camera> camera)
        : _camera(camera)
        {}

        Circle relativeCircleOf(std::shared_ptr<Singuity> singuity)
        {
            return Circle(_camera->absolutePositionToRelative(singuity->position()), _camera->absoluteLengthToRelative(ABSOLUTE_SINGUITY_RADIUS));
        }

        Circle relativeCircleOf(std::shared_ptr<Spawner> spawner)
        {
            return Circle(_camera->absolutePositionToRelative(spawner->position()), _camera->absoluteLengthToRelative(ABSOLUTE_SPAWNER_RADIUS));
        }

        double absoluteLengthToRelative(const double& length)
        {
            return _camera->absoluteLengthToRelative(length);
        }

        Vector2D relativePositionToAbsolute(const Vector2D& position)
        {
            return _camera->relativePositionToAbsolute(position);
        }

        Vector2D absolutePositionToRelative(const Vector2D& position)
        {
            return _camera->absolutePositionToRelative(position);
        }

    private:
        static const double ABSOLUTE_SINGUITY_RADIUS;
        static const double ABSOLUTE_SPAWNER_RADIUS;

        const std::shared_ptr<Camera> _camera;
    };
}