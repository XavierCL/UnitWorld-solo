#pragma once

#include "MobileUnit.h"

namespace uw
{
	class Singuity : public MobileUnit
	{
	public:
		Singuity(const Vector2D& initialPosition):
			MobileUnit(initialPosition)
		{}

	private:
		const double maximumSpeed() const
		{
			return 1.5;
		}
		const double maximumAcceleration() const
		{
			return 0.02;
		}
	};
}