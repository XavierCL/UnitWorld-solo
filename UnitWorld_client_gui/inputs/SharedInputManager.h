#pragma once

#include "game/physics/Vector2.h"
#include "game/physics/Vector2D.h"

#include <mutex>

namespace uw
{
	class SharedInputManager
	{
	public:
		const bool hasClicked()
		{
			if(_clickedMutex.try_lock())
			{
				const bool hasClicked = _hasClicked;
				_clickedMutex.unlock();
				return hasClicked;
			}
			else
			{
				return false;
			}
		}
		const Vector2D clickedAt()
		{
			if(_clickedMutex.try_lock())
			{
			}
			else
		}
		void clickedAt(const Vector2& position);
		const bool hasReleasedMouse();
		const bool isMouseDown();
		const Vector2D mouseFrom();
		const Vector2D mouseTo();
		void mouseFrom(const Vector2& position);
		void mouseTo(const Vector2& position);
	private:
		bool _hasClicked;
		std::mutex _clickedMutex;
	};
}