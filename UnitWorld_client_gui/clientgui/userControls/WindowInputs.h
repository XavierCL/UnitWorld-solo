#pragma once

#include "UserControlState.h"
#include "clientgui/cameras/Camera.h"

#include <memory>

namespace uw
{
    class WindowInputs
    {
    public:
        WindowInputs(std::shared_ptr<UserControlState> userControlState, std::shared_ptr<Camera> camera)
        : _userControlState(userControlState),
        _camera(camera)
        {}

        void frameHappened()
        {
            _userControlState->frameHappened();
            _camera->frameHappened();
        }

        void setUserMousePosition(const Vector2D& position)
        {
            _userControlState->setUserMousePosition(position);
            _camera->mouseMovedTo(position);
        }

        void setUserLeftMouseUpPosition(const Vector2D& position)
        {
            _userControlState->setUserLeftMouseUpPosition(position);
        }

        void setUserLeftMouseDownPosition(const Vector2D& position)
        {
            _userControlState->setUserLeftMouseDownPosition(position);
        }

        void setUserRightMouseDownPosition(const Vector2D& position)
        {
            _userControlState->setUserRightMouseDownPosition(position);
        }

        void setUserMouseWheelScroll(const double& delta, const Vector2D& mousePosition)
        {
            _camera->mouseScrolled(delta, mousePosition);
        }

    private:
        const std::shared_ptr<UserControlState> _userControlState;
        const std::shared_ptr<Camera> _camera;
    };
}