#pragma once

#include <shared/game/geometry/Rectangle.h>
#include <shared/game/geometry/Vector2D.h>

#include <memory>
#include <algorithm>

namespace uw
{
    class Camera
    {
    public:
        Camera(const double& worldAbsoluteWidth, const double& worldAbsoluteHeight, const Rectangle& screenRelativeRectangle, const double& sidePanelWidthRatio, const double& translationPixelPerFrame, const double& scaleRatioPerTick) :
            _worldAbsoluteWidth(worldAbsoluteWidth),
            _worldAbsoluteHeight(worldAbsoluteHeight),
            _screenRelativeRectangle(screenRelativeRectangle),
            _mouseSafeZone(screenRelativeRectangle.smallerBy(sidePanelWidthRatio * screenRelativeRectangle.width())),
            _translationPixelPerFrame(translationPixelPerFrame),
            _scaleRatioPerTick(scaleRatioPerTick),
            _lastMousePosition(screenRelativeRectangle.center()),
            _centerAbsolutePosition(std::make_shared<Vector2D>(worldAbsoluteWidth / 2.0, worldAbsoluteHeight / 2.0)),
            _absoluteScale(maximumAbsoluteScale(worldAbsoluteWidth, worldAbsoluteHeight, screenRelativeRectangle))
        {}

        void mouseMoved(const Vector2D& position)
        {
            _lastMousePosition = position;
        }

        void frameHappened()
        {
            Vector2D distanceToMove;
            Vector2D distanceThreshold = (_screenRelativeRectangle.size() - _mouseSafeZone.size()) / 2.0;
            if (!_mouseSafeZone.contains(_lastMousePosition))
            {
                if (_screenRelativeRectangle.lowerRightCorner().x() - _lastMousePosition.x() < distanceThreshold.x())
                {
                    distanceToMove += Vector2D(_translationPixelPerFrame, 0.0);
                }
                else if (_lastMousePosition.x() - distanceThreshold.x() < _screenRelativeRectangle.upperLeftCorner().x())
                {
                    distanceToMove += Vector2D(-_translationPixelPerFrame, 0.0);
                }

                if (_screenRelativeRectangle.lowerRightCorner().y() - _lastMousePosition.y() < distanceThreshold.y())
                {
                    distanceToMove += Vector2D(0, _translationPixelPerFrame);
                }
                else if (_lastMousePosition.y() - distanceThreshold.y() < _screenRelativeRectangle.upperLeftCorner().y())
                {
                    distanceToMove += Vector2D(0, -_translationPixelPerFrame);
                }

                distanceToMove.maxAt(_translationPixelPerFrame);

                distanceToMove = Vector2D(
                    std::max(std::min(distanceToMove.x(), relativePositionToAbsolute(Vector2D(_worldAbsoluteWidth, 0.0)).x() - _screenRelativeRectangle.lowerRightCorner().x()), _screenRelativeRectangle.upperLeftCorner().x() - relativePositionToAbsolute(Vector2D(0.0, 0.0)).x()),
                    std::max(std::min(distanceToMove.y(), relativePositionToAbsolute(Vector2D(0.0, _worldAbsoluteHeight)).y() - _screenRelativeRectangle.lowerRightCorner().y()), _screenRelativeRectangle.upperLeftCorner().y() - relativePositionToAbsolute(Vector2D(0.0, 0.0)).y())
                );

                _centerAbsolutePosition = std::make_shared<Vector2D>(*_centerAbsolutePosition + distanceToMove.atModule(relativeLengthToAbsolute(distanceToMove.module())));
            }
        }

        void mouseScrolled(const double& scrollDelta, const Vector2D& mousePosition)
        {
            const auto mouseAbsolutePositionRelativeToCenter = mousePosition - _screenRelativeRectangle.center();
            double newAbsoluteScale = _absoluteScale + _absoluteScale * scrollDelta * _scaleRatioPerTick;
            _absoluteScale = std::max(std::min(newAbsoluteScale, maximumAbsoluteScale(_worldAbsoluteWidth, _worldAbsoluteHeight, _screenRelativeRectangle));

            Vector2D newAbsolutCenter(*_centerAbsolutePosition + mouseAbsolutePositionRelativeToCenter.atModule(relativeLengthToAbsolute(mouseAbsolutePositionRelativeToCenter.module())));

            newAbsoluteCenter = 

            _centerAbsolutePosition = std::make_shared<Vector2D>();
        }

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
            return _screenRelativeRectangle.center() + (relativePositionWithoutScale * _absoluteScale);
        }

        Vector2D relativePositionToAbsolute(const Vector2D& relativePosition)
        {
            Vector2D relativePositionWithAbsoluteScale = (relativePosition - _screenRelativeRectangle.center()) / _absoluteScale;
            return relativePositionWithAbsoluteScale + *_centerAbsolutePosition;
        }

    private:
        static double maximumAbsoluteScale(const double& worldAbsoluteWidth, const double& worldAbsoluteHeight, const Rectangle& screenRelativeRectangle)
        {
            if (worldAbsoluteWidth / screenRelativeRectangle.size().x() < worldAbsoluteHeight / screenRelativeRectangle.size().y())
            {
                return worldAbsoluteHeight / screenRelativeRectangle.size().y();
            }
            else
            {
                return worldAbsoluteWidth / screenRelativeRectangle.size().x();
            }
        }

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