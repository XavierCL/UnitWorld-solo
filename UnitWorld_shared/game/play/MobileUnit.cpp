#include "MobileUnit.h"

using namespace uw;

MobileUnit::MobileUnit() :
	_destination(nullptr)
{}

MobileUnit::~MobileUnit()
{
	delete _destination;
}

void MobileUnit::setDestination(const Point& destination)
{
	delete _destination;
	_destination = new Point(destination);
}