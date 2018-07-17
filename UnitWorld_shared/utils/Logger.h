#pragma once

#include <cstring>
#include <vector>
#include <functional>

class Logger
{
public:
	static void info(const std::string &message)
	{
		outputString(message, _infoOutputs);
	}

	static void registerInfo(const std::function<void(const std::string&)>& out)
	{
		_infoOutputs.push_back(out);
	}

private:
	static std::vector<std::function<void(const std::string&)>> _infoOutputs;

	static void outputString(const std::string &message, const std::vector<std::function<void(const std::string&)>> &outputs)
	{
		for (auto &output : outputs)
		{
			output(message);
		}
	}
};