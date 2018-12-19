#pragma once

#include "Logger.hpp"

#include <random>
#include <mutex>
#include <string>

class RandomEngineGenerator
{
public:

	RandomEngineGenerator(const unsigned int& seed)
		: _underlying(seed)
	{}

	std::minstd_rand0 next()
	{
		std::lock_guard<std::mutex> lock(singlePassageMutex);
		std::minstd_rand0 nextEngine(_underlying);
		_underlying.seed(_underlying());
		return nextEngine;
	}

private:
	std::mutex singlePassageMutex;
	std::minstd_rand0 _underlying;
};