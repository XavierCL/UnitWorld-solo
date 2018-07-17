#pragma once

#include <string>

class Command
{
public:
	virtual std::string rawType() const = 0;
	virtual std::string rawData() const = 0;
};