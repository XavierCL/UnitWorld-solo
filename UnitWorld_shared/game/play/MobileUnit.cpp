#include "MobileUnit.h"

using namespace uw;

MobileUnit::MobileUnit() :
	_destination(nullptr)
{}

MobileUnit::~MobileUnit()
{
	delete _destination;
}

void MobileUnit::setDestination(const Vector2& destination)
{
	delete _destination;
	_destination = new Vector2(destination);
}