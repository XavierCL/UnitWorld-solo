#pragma once

#include "game/physics/Vector2.h"

#include "utils/Option.hpp"

#include <queue>
#include <mutex>

namespace uw
{
	class SharedInputManager
	{
	public:
		const Option<Vector2> clickedAt()
		{
			if(_clickedMutex.try_lock())
			{
				if(!_clicks.empty())
				{
					Vector2 click = _clicks.back();
					_clicks.pop();
					return Option<Vector2>(click);
				}
			}
			else
			{
				return Option<Vector2>::None;
			}
		}

		void clickedAt(const Vector2& position)
		{
			std::lock_guard<std::mutex> lock(_clickedMutex);
			_clicks.push(position);
		}

		const bool hasReleasedMouse()
		{

		}

		const bool isMouseDown();
		const Vector2 mouseFrom();
		const Vector2 mouseTo();
		void mouseFrom(const Vector2& position);
		void mouseTo(const Vector2& position);
	private:
		Option<Vector2> _mouseCurrentTo;
		Option<Vector2> _mouseFrom;
		std::queue<Vector2> _clicks;

		std::mutex _clickedMutex;
	};
}
