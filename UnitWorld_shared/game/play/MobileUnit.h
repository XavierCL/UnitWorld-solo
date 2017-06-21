#pragma once

#include "Unit.h"

namespace uw
{
	class MobileUnit: public Unit
	{
	public:
		void move(const Point& destination);
	private:
		int _xSpeed;
		int _ySpeed;
	};
}