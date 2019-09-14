#pragma once

#include "shared/game/GameManager.h"

#include "Camera.h"

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
            return Circle(_camera->absolutePositionToRelative(singuity.position, ABSOLUTE_SINGUITY_RADIUS)
        }

        Circle relativeCircleOf(std::shared_ptr<Spawner> spawner)
        {
            return Circle(_camera->absolutePositionToRelative(position), ABSOLUTE_SPAWNER_RADIUS)
        }

    private:
        static const double ABSOLUTE_SINGUITY_RADIUS;
        static const double ABSOLUTE_SPAWNER_RADIUS;

        const std::shared_ptr<Camera> _camera;
    };
}