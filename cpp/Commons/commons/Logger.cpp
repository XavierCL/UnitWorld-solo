#include "Logger.hpp"

std::vector<std::function<void(const std::string&)>> Logger::_errorOutputs;
std::vector<std::function<void(const std::string&)>> Logger::_infoOutputs;
std::vector<std::function<void(const std::string&)>> Logger::_traceOutputs;
