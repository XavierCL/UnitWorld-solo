#pragma once

#include "Unit.h"
#include "MobileUnitDestination.h"

namespace uw
{
    class MobileUnit : virtual public Unit
    {
    public:

        Vector2D speed() const
        {
            return _speed;
        }

        Option<MobileUnitDestination> destination() const
        {
            return _destination;
        }

        virtual double maximumSpeed() const = 0;
        virtual double maximumAcceleration() const = 0;

        double stopDistanceFromTargetSq() const
        {
            const auto stopDistance = stopDistanceFromTarget();
            return stopDistance * stopDistance;
        }

        Vector2D getMaximalAcceleration(const Vector2D& destination)
        {
            Vector2D stopDelta = _speed.atModule(stopDistanceFromTarget() * 2);
            Vector2D expectedPosition(position() + stopDelta);
            return (destination - expectedPosition).maxAt(maximumAcceleration());
        }

        Vector2D getBreakingAcceleration() const
        {
            return Vector2D(-_speed.x(), -_speed.y()).maxAt(maximumAcceleration());
        }

        Vector2D getSlowBreakingAcceleration() const
        {
            return Vector2D(-_speed.x(), -_speed.y()).maxAt(maximumAcceleration() * 0.05);
        }

        bool hasSpawnerDestination() const
        {
            return _destination
                .map<bool>([](const MobileUnitDestination& destination) { return destination.isSpawnerDestination(); })
                .getOrElse(false);
        }

        void setPointDestination(const Vector2D& destination)
        {
            _destination = Options::Some(MobileUnitDestination(destination));
        }

        void setSpawnerDestination(const SpawnerDestination& spawnerDestination)
        {
            _destination = Options::Some(MobileUnitDestination(spawnerDestination));
        }

        void clearDestination()
        {
            _destination = Options::None<MobileUnitDestination>();
        }

        void actualizeSpeed(const Vector2D& instantaneousAcceleration)
        {
            _speed = (_speed + instantaneousAcceleration).maxAt(maximumSpeed());
            position(position() + _speed);
        }

        void actualizePosition(const Vector2D& instantaneousSpeed)
        {
            position(position() + instantaneousSpeed);
        }

    protected:
        MobileUnit(const xg::Guid& id, const Vector2D& position, const Vector2D& speed, const Option<MobileUnitDestination>& destination) :
            Unit(id, position),
            _speed(speed),
            _destination(destination)
        {}

        MobileUnit(const Vector2D& position, const Vector2D& speed) :
            Unit(position),
            _speed(speed),
            _destination()
        {}

        MobileUnit(const Vector2D& position, const Vector2D& speed, const Option<MobileUnitDestination>& destination) :
            Unit(position),
            _speed(speed),
            _destination(destination)
        {}

        MobileUnit(const Vector2D& position) :
            Unit(position),
            _speed(0.0),
            _destination()
        {}

        MobileUnit(const MobileUnit& copy) :
            Unit(copy),
            _speed(copy._speed),
            _destination(copy._destination)
        {}

    private:

        double stopDistanceFromTarget() const
        {
            return _speed.moduleSq() / (2.0 * maximumAcceleration());
        }

        Option<MobileUnitDestination> _destination;
        Vector2D _speed;
    };
}