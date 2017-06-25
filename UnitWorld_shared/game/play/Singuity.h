#pragma once

#include "MobileUnit.h"

namespace uw
{
	class Singuity : public MobileUnit
	{
	public:
		Singuity(const Vector2& position):
			MobileUnit(position)
		{}
	};
}