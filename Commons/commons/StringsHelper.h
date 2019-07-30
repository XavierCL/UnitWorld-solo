#pragma once

#include <vector>

class StringsHelper
{
public:
    static std::vector<std::string> split(const std::string& original, const std::string& delimiter)
    {
        std::vector<std::string> cont;
        std::size_t current, previous = 0;
        current = original.find(delimiter);
        while (current != std::string::npos) {
            cont.push_back(original.substr(previous, current - previous));
            previous = current + delimiter.size();
            current = original.find(delimiter, previous);
        }
        cont.push_back(original.substr(previous, current - previous));

        return cont;
    }
};