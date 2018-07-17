#pragma once

#include "DisplayCanvas.h"
#include "DrawingCanvas.h"

namespace uw
{
	class DisplayDrawerCanvas : public DisplayCanvas, public DrawingCanvas
	{};
}
