#pragma once

#include <shared/game/geometry/Vector2D.h>

#include <memory>

namespace uw
{
    class Camera
    {
    public:
        Camera(const double& worldAbsoluteWidth, const double& worldAbsoluteHeight, const double& translationPixelPerFrame, const double& scaleRatioPerTick)
        {}

        void mouseMove();

        void frameHappened();

        void mouseTicked();

        double absoluteLengthToRelative(const double& length)
        {
            return length * _absoluteScale;
        }

        double relativeLengthToAbsolute();

        Vector2D absolutePositionToRelative();

        Vector2D relativePositionToAbsolute();

    private:
        std::shared_ptr<Vector2D> _centerAbsolutePosition;
        double _absoluteScale;
    };
}