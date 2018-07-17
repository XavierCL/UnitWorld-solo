#include "Logger.h"

std::vector<std::function<void(const std::string&)>> Logger::_infoOutputs;