#include "MobileUnit.h"

#include <cmath>

using namespace uw;

MobileUnit::~MobileUnit()
{
	delete _destination;
}

void MobileUnit::actualize()
{
	if(_destination)
	{
		if((position().x() - _destination->x()) * (position().x() - _destination->x()) +
			(position().y() - _destination->y()) * (position().y() - _destination->y()) >=
			16)
		{
			_speed.x(abs(position().x() - _destination->x()) > abs(position().y() - _destination->y()));
			if (position().x() - _destination->x() > 0)
				_speed.x(-_speed.x());
			_speed.y(abs(position().x() - _destination->x()) <= abs(position().y() - _destination->y()));
			if (position().y() - _destination->y() > 0)
				_speed.y(-_speed.y());
		}
		else
		{
			_speed.x(0);
			_speed.y(0);
			delete _destination;
			_destination = nullptr;
		}
	}
	position().x(position().x() + _speed.x());
	position().y(position().y() + _speed.y());
}

void MobileUnit::setDestination(const Vector2& destination)
{
	delete _destination;
	_destination = new Vector2(destination);
}

uw::MobileUnit::MobileUnit(const MobileUnit & copy):
	Unit(copy)
{
	if(copy._destination)
	{
		_destination = new Vector2(*copy._destination);
	}
	else
	{
		_destination = nullptr;
	}
	_speed = copy._speed;
}

MobileUnit::MobileUnit(const Vector2& initialPosition) :
	Unit(initialPosition),
	_destination(nullptr)
{}