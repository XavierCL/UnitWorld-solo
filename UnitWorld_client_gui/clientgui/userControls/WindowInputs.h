#pragma once

#include "UserControlState.h"
#include "clientgui/cameras/Camera.h"

#include <memory>

namespace uw
{
    class WindowInputs
    {
    public:
        WindowInputs(std::shared_ptr<UserControlState> userControlState, std::shared_ptr<Camera> camera, const Rectangle& screenRelativeRectangle) :
            _userControlState(userControlState),
            _camera(camera),
            _isLeftControlKeyPressed(false),
            _isRapidClicking(false),
            _justClicked(false),
            _screenRelativeRectangle(screenRelativeRectangle)
        {}

        void frameHappened()
        {
            _userControlState->frameHappened();
            _camera->frameHappened();
        }

        void setUserMousePosition(const Vector2D& position)
        {
            _isRapidClicking = false;
            _justClicked = false;
            _userControlState->setUserMousePosition(position);
            _camera->mouseMovedTo(position);
        }

        void setUserLeftMouseUpPosition(const Vector2D& position)
        {
            if (_justClicked && _isRapidClicking)
            {
                _userControlState->setUserDoubleClickMouseUpPosition(position);
            }
            else if (_isRapidClicking)
            {
                _userControlState->setUserClickMouseUpPosition(position, _isLeftControlKeyPressed);
            }
            else
            {
                _userControlState->setUserLeftMouseUpPosition(position);
            }
            _justClicked = _isRapidClicking;
            _isRapidClicking = false;
        }

        void setUserLeftMouseDownPosition(const Vector2D& position)
        {
            _isRapidClicking = true;
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

		void userPressedLeftControl()
		{
			_isLeftControlKeyPressed = true;
		}

		void userReleasedLeftControl()
		{
			_isLeftControlKeyPressed = false;
		}

		void userPressedLeftShift()
		{
            _userControlState->userPressedLeftShift();
		}

		void userReleasedLeftShift()
		{
            _userControlState->userReleasedLeftShift();
		}

		void userPressedNumber(const int& numberKey)
		{
			if (_isLeftControlKeyPressed)
			{
				_userControlState->setSelectedUnitsToUnitGroup(numberKey);
			}
			else
			{
				_userControlState->setUnitGroupToSelectedUnits(numberKey);
			}
		}

        void userPressedAKey()
        {
            _userControlState->selectAllUnits();
        }

        void userPressedAddKey()
        {
            _camera->mouseScrolled(1, _screenRelativeRectangle.center());
        }

        void userPressedSubstractKey()
        {
            _camera->mouseScrolled(-1, _screenRelativeRectangle.center());
        }

    private:
        const std::shared_ptr<UserControlState> _userControlState;
        const std::shared_ptr<Camera> _camera;
        const Rectangle _screenRelativeRectangle;
		bool _isLeftControlKeyPressed;
        bool _isRapidClicking;
        bool _justClicked;
    };
}