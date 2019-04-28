#include "Logger.hpp"

std::vector<std::function<void(const std::string&)>> Logger::_infoOutputs;
std::vector<std::function<void(const std::string&)>> Logger::_traceOutputs;
