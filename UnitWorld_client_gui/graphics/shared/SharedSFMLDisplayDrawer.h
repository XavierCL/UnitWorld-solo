#pragma once

#include <atomic>
#include "SharedDrawer.h"
#include "SharedDisplay.h"
#include "../canvas/DisplayDrawerCanvas.h"

namespace uw
{
	class SharedSFMLDislayDrawer: public SharedDrawer, public SharedDisplay
	{
	public:
		SharedSFMLDislayDrawer(DisplayDrawerCanvas& canvas) :
			_canvas(canvas),
			_shouldCloseCanvas(false)
		{}

		void tryDrawingTransaction(const std::function<void(DrawingCanvas&)>& drawingFunction)
		{
			if(!_shouldCloseCanvas)
			{
				drawingFunction(_canvas);
				_canvas.display();
			}
		}

		void requestClosure()
		{
			_shouldCloseCanvas = true;
		}

		void tryClose()
		{
			if(_shouldCloseCanvas)
			{
				_canvas.close();
			}
		}
		
	private:
		DisplayDrawerCanvas& _canvas;
		std::atomic<bool> _shouldCloseCanvas;
	};
}