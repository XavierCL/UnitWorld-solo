#pragma once

#include <cstring>
#include <vector>
#include <functional>

class Logger
{
public:
    static void error(const std::string& message)
    {
        outputString(message, _errorOutputs);
    }

    static void info(const std::string &message)
    {
        outputString(message, _infoOutputs);
    }

    static void trace(const std::string &message)
    {
        outputString(message, _traceOutputs);
    }

    static void registerError(const std::function<void(const std::string&)>& out)
    {
        _errorOutputs.push_back(out);
    }

    static void registerInfo(const std::function<void(const std::string&)>& out)
    {
        _infoOutputs.push_back(out);
    }

    static void registerTrace(const std::function<void(const std::string&)>& out)
    {
        _traceOutputs.push_back(out);
    }

private:
    static std::vector<std::function<void(const std::string&)>> _errorOutputs;
    static std::vector<std::function<void(const std::string&)>> _infoOutputs;
    static std::vector<std::function<void(const std::string&)>> _traceOutputs;

    static void outputString(const std::string &message, const std::vector<std::function<void(const std::string&)>> &outputs)
    {
        for (auto &output : outputs)
        {
            output(message);
        }
    }
};