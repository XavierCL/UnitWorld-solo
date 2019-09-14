#pragma once

#include <shared/game/geometry/Rectangle.h>
#include <shared/game/geometry/Vector2D.h>

#include <memory>

namespace uw
{
    class Camera
    {
    public:
        Camera(const double& worldAbsoluteWidth, const double& worldAbsoluteHeight, const Rectangle& screenRelativeRectangle, const double& sidePanelWidth, const double& translationPixelPerFrame, const double& scaleRatioPerTick):
            _worldAbsoluteWidth(worldAbsoluteWidth),
            _worldAbsoluteHeight(worldAbsoluteHeight),
            _screenRelativeRectangle(screenRelativeRectangle),
            _mouseSafeZone(screenRelativeRectangle.smallerBy(sidePanelWidth)),
            _translationPixelPerFrame(translationPixelPerFrame),
            _scaleRatioPerTick(scaleRatioPerTick),
            _lastMousePosition(screenRelativeRectangle.center())
        {}

        void mouseMoved(const Vector2D& position)
        {

        }

        void frameHappened();

        void mouseTickedUp();

        void mouseTickedDown();

        double absoluteLengthToRelative(const double& length)
        {
            return length * _absoluteScale;
        }

        double relativeLengthToAbsolute();

        Vector2D absolutePositionToRelative();

        Vector2D relativePositionToAbsolute();

    private:
        const double _worldAbsoluteWidth;
        const double _worldAbsoluteHeight;
        const Rectangle _screenRelativeRectangle;
        const double _translationPixelPerFrame;
        const double _scaleRatioPerTick;

        Vector2D _lastMousePosition;

        std::shared_ptr<Vector2D> _centerAbsolutePosition;
        double _absoluteScale;
    };
}