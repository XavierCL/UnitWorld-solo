#pragma once

#include "Unit.h"

#include <cmath>

namespace uw
{
	class MobileUnit: public Unit
	{
	public:
		~MobileUnit();

		virtual void actualize();
		void setDestination(const Vector2D& destination);
	protected:
		MobileUnit(const MobileUnit& copy);
		MobileUnit(const Vector2D& initialPosition);

	private:
		void setMaximalAcceleration(const Vector2D& destination);
		const double stopDistanceFromTarget() const
		{
			sqrt(stopDistanceFromTargetSq());
		}
		const double stopDistanceFromTargetSq() const
		{
			const auto speedModule = _speed.module();
			if(speedModule == 0)
			{
				return 0;
			}
			const auto relativeX = abs(_speed.x()) / speedModule;
			const auto relativeY = abs(_speed.y()) / speedModule;
			const auto accX = maximumAcceleration() * relativeX;
			const auto accY = maximumAcceleration() * relativeY;

			const auto numberOfMeterX = (_speed.x() / accX) * (_speed.x() / 2);
			const auto numberOfMeterY = (_speed.y() / accY) * (_speed.y() / 2);

			return Vector2D(numberOfMeterX, numberOfMeterY).moduleSq() + 1;
		}
		Vector2D getBreakingAcceleration() const;
		void deleteDestination();
		virtual const double maximumSpeed() const = 0;
		virtual const double maximumAcceleration() const = 0;

		Vector2D* _destination;
		Vector2D _speed;
		Vector2D _acceleration;
	};
}