#pragma once

#include <shared/game/geometry/Rectangle.h>
#include <shared/game/geometry/Vector2D.h>

#include <memory>

namespace uw
{
    class Camera
    {
    public:
        Camera(const double& worldAbsoluteWidth, const double& worldAbsoluteHeight, const Rectangle& screenRelativeRectangle, const double& sidePanelWidthRatio, const double& translationPixelPerFrame, const double& scaleRatioPerTick):
            _worldAbsoluteWidth(worldAbsoluteWidth),
            _worldAbsoluteHeight(worldAbsoluteHeight),
            _screenRelativeRectangle(screenRelativeRectangle),
            _mouseSafeZone(screenRelativeRectangle.smallerBy(sidePanelWidthRatio)),
            _translationPixelPerFrame(translationPixelPerFrame),
            _scaleRatioPerTick(scaleRatioPerTick),
            _lastMousePosition(screenRelativeRectangle.center())
        {}

        void mouseMoved(const Vector2D& position)
        {
            _lastMousePosition = position;
        }

        void frameHappened()
        {
            if (!_mouseSafeZone.contains(_lastMousePosition))
            {
                if()
            }
        }

        void mouseScrolled(const double& scrollDelta, const Vector2D& mousePosition);

        double absoluteLengthToRelative(const double& length)
        {
            return length * _absoluteScale;
        }

        double relativeLengthToAbsolute(const double& length)
        {
            return length / _absoluteScale;
        } 

        Vector2D absolutePositionToRelative(const Vector2D& absolutePosition)
        {
            Vector2D relativePositionWithoutScale = absolutePosition - *_centerAbsolutePosition;
            return relativePositionWithoutScale * _absoluteScale;
        }

        Vector2D relativePositionToAbsolute(const Vector2D& relativePosition)
        {
            Vector2D relativePositionWithAbsoluteScale = relativePosition / _absoluteScale;
            return relativePositionWithAbsoluteScale + *_centerAbsolutePosition;
        }

    private:
        const double _worldAbsoluteWidth;
        const double _worldAbsoluteHeight;
        const Rectangle _screenRelativeRectangle;
        const double _translationPixelPerFrame;
        const double _scaleRatioPerTick;
        const Rectangle _mouseSafeZone;

        Vector2D _lastMousePosition;

        std::shared_ptr<Vector2D> _centerAbsolutePosition;
        double _absoluteScale;
    };
}