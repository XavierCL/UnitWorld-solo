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
            _absoluteScale(minimumAbsoluteScale(worldAbsoluteWidth, worldAbsoluteHeight, screenRelativeRectangle))
        {}

        void mouseMovedTo(const Vector2D& position)
        {
            _lastMousePosition = position;
        }

        void frameHappened()
        {
            if (!_mouseSafeZone.contains(_lastMousePosition))
            {
                Vector2D distanceToMove;
                Vector2D distanceThreshold = (_screenRelativeRectangle.size() - _mouseSafeZone.size()) / 2.0;

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

                distanceToMove = distanceToMove.maxAt(_translationPixelPerFrame);

                const double absoluteOutboundWidth = relativeLengthToAbsolute(_screenRelativeRectangle.size().x() / 2.0);
                const double absoluteOutboundHeight = relativeLengthToAbsolute(_screenRelativeRectangle.size().y() / 2.0);
                const Rectangle inbountAbsoluteCenter(Vector2D(absoluteOutboundWidth, absoluteOutboundHeight), Vector2D(_worldAbsoluteWidth - absoluteOutboundWidth, _worldAbsoluteHeight - absoluteOutboundHeight));

                _centerAbsolutePosition = std::make_shared<Vector2D>(inbountAbsoluteCenter.closestPointTo(*_centerAbsolutePosition + (distanceToMove / _absoluteScale)));
            }
        }

        void mouseScrolled(const double& scrollDelta, const Vector2D& mousePosition)
        {
            const auto mousePositionRelativeToCenter = mousePosition - _screenRelativeRectangle.center();
            const auto absoluteMousePosition = relativePositionToAbsolute(mousePosition);

            double newAbsoluteScale = _absoluteScale + _absoluteScale * scrollDelta * _scaleRatioPerTick;
            _absoluteScale = std::min(std::max(newAbsoluteScale, minimumAbsoluteScale(_worldAbsoluteWidth, _worldAbsoluteHeight, _screenRelativeRectangle)), maximumAbsoluteScale());

            const double absoluteOutboundWidth = relativeLengthToAbsolute(_screenRelativeRectangle.size().x() / 2.0);
            const double absoluteOutboundHeight = relativeLengthToAbsolute(_screenRelativeRectangle.size().y() / 2.0);
            const Rectangle inbountAbsoluteCenter(Vector2D(absoluteOutboundWidth, absoluteOutboundHeight), Vector2D(_worldAbsoluteWidth - absoluteOutboundWidth, _worldAbsoluteHeight - absoluteOutboundHeight));

            _centerAbsolutePosition = std::make_shared<Vector2D>(inbountAbsoluteCenter.closestPointTo(absoluteMousePosition - (mousePositionRelativeToCenter / _absoluteScale)));
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
        static double minimumAbsoluteScale(const double& worldAbsoluteWidth, const double& worldAbsoluteHeight, const Rectangle& screenRelativeRectangle)
        {
            if (worldAbsoluteWidth / screenRelativeRectangle.size().x() > worldAbsoluteHeight / screenRelativeRectangle.size().y())
            {
                return screenRelativeRectangle.size().x() / worldAbsoluteWidth;
            }
            else
            {
                return screenRelativeRectangle.size().y() / worldAbsoluteHeight;
            }
        }

        static double maximumAbsoluteScale()
        {
            return 20;
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