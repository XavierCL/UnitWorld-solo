#pragma once

#include "../canvas/DrawingCanvas.h"

#include <functional>

namespace uw
{
	class SharedDrawer
	{
	public:
		virtual void tryDrawingTransaction(const std::function<void(DrawingCanvas&)>& drawingFunction) = 0;

		virtual void requestClosure() = 0;
	};
}